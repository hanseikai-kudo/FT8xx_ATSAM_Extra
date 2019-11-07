
#include <string.h>
#include <math.h>


#define FACE_BACK 0
#define FACE_FRONT 1

#define SCREEN_WX 480
#define SCREEN_WY 272

#define P 125
#define N -P

#define M_PI 3.14159265358979323846
#define PIXELS(x)  (int)( (x) * 16)

static const signed char CUBE_vertices[] =
{
  P,P,P,
  N,P,P,
  P,N,P,
  N,N,P,

  P,P,N,
  N,P,N,
  P,N,N,
  N,N,N,
};

// each line is a face: count, normal, 4 vertices

static const signed char CUBE_faces[] =
{

  4, 0,0,127,   0, 1, 3, 2,
  4, 0,0,-127,  6, 7, 5, 4,

  4, 0,127,0,   4, 5, 1, 0,
  4, 0,-127,0,  2, 3, 7, 6,

  4, 127,0,0,   0, 2, 6, 4,
  4, -127,0,0,  3, 1, 5, 7,

  -1
};

////////////////////////////////////////////////////////////////////////////////
//                                  3D Projection
////////////////////////////////////////////////////////////////////////////////

static float model_mat[9] =
{
  1.0, 0.0, 0.0,
  0.0, 1.0, 0.0,
  0.0, 0.0, 1.0
};

static float normal_mat[9] =
{
  1.0, 0.0, 0.0,
  0.0, 1.0, 0.0,
  0.0, 0.0, 1.0
};

#define M(nm,i,j)       ((nm)[3 * (i) + (j)])

// 3x3 matrix multiplication: c = a * b
void mult_matrices(float *a, float *b, float *c)
{
  int i, j, k;
  float result[9];

  for(i = 0; i < 3; i++)
  {
    for(j = 0; j < 3; j++)
    {
//      M(result,i,j) = 0.0f;
	  result[3*i+j] = 0.0f;
      for(k = 0; k < 3; k++)
      {
//        M(result,i,j) += M(a,i,k) * M(b,k,j);
		  result[3*i+j] += a[3*i+k] * b[3*k+j];
      }
    }
  }
  memcpy(c, result, sizeof(result));
}

// Based on glRotate()
// Returns 3x3 rotation matrix in 'm'
// and its invese in 'mi'

static void rotate(float *m, float *mi, float angle, float *axis)
{
  float x = axis[0];
  float y = axis[1];
  float z = axis[2];

  float s = sin(angle);
  float c = cos(angle);

  float xx = x*x*(1-c);
  float xy = x*y*(1-c);
  float xz = x*z*(1-c);
  float yy = y*y*(1-c);
  float yz = y*z*(1-c);
  float zz = z*z*(1-c);

  float xs = x * s;
  float ys = y * s;
  float zs = z * s;

  m[0] = xx + c;
  m[1] = xy - zs;
  m[2] = xz + ys;

  m[3] = xy + zs;
  m[4] = yy + c;
  m[5] = yz - xs;

  m[6] = xz - ys;
  m[7] = yz + xs;
  m[8] = zz + c;

  mi[0] = m[0];
  mi[1] = xy + zs;
  mi[2] = xz - ys;

  mi[3] = xy - zs;
  mi[4] = m[4];
  mi[5] = yz + xs;

  mi[6] = xz + ys;
  mi[7] = yz - xs;
  mi[8] = m[8];
}

static void rotation(float angle, float *axis)
{
  float mat[9], mati[9];

  rotate( &mat[0], &mati[0], angle, &axis[0] );
  mult_matrices( &model_mat[0], &mat[0], &model_mat[0] );
  mult_matrices( &mati[0], &normal_mat[0], &normal_mat[0] );
}

#define N_VERTICES  (sizeof(CUBE_vertices) / 3)

typedef struct {
  int x, y;
} point2;

static point2 projected[N_VERTICES];

void project( void )
{
//  unsigned char vx;
  signed char *pm = &CUBE_vertices[0];
  signed char *pm_e = pm + sizeof(CUBE_vertices);
  point2 *dst = &projected[0];
  short x, y, z;
  int scale = 64 * SCREEN_WY / 280;

  while (pm < pm_e)
  {
    x = (scale * (signed char)(*pm++)) >> 6;
    y = (scale * (signed char)(*pm++)) >> 6;
    z = (scale * (signed char)(*pm++)) >> 6;

    float xx = x * model_mat[0] + y * model_mat[3] + z * model_mat[6];
    float yy = x * model_mat[1] + y * model_mat[4] + z * model_mat[7];

    dst->x = 16 * (SCREEN_WX / 2 + xx);
    dst->y = 16 * (SCREEN_WY / 2 + yy);
    dst++;
  }
}

static void transform_normal( signed char *nx, signed char *ny, signed char *nz)
{
  signed char xx = *nx * normal_mat[0] + *ny * normal_mat[1] + *nz * normal_mat[2];
  signed char yy = *nx * normal_mat[3] + *ny * normal_mat[4] + *nz * normal_mat[5];
  signed char zz = *nx * normal_mat[6] + *ny * normal_mat[7] + *nz * normal_mat[8];
  *nx = xx, *ny = yy, *nz = zz;
}

static void quad(int x1, int y1,
                 int x2, int y2,
                 int x3, int y3,
                 int bx1, int by1,
                 int bx3, int by3)
{
  // Compute the fourth vertex of the parallelogram, (x4,y4)
  int x4 = x3 + (x1 - x2);
  int y4 = y3 + (y1 - y2);

  // Apply Scissor to the extents of the quad
  int minx = max(0,    min(min(x1, x2), min(x3, x4)));
  int maxx = min(SCREEN_WX, max(max(x1, x2), max(x3, x4)));
  int miny = max(0,    min(min(y1, y2), min(y3, y4)));
  int maxy = min(SCREEN_WY, max(max(y1, y2), max(y3, y4)));

	RamMemWriteDoubleWord( SCISSOR_XY(minx, miny) );
	RamMemWriteDoubleWord( SCISSOR_SIZE( (maxx - minx), (maxy - miny) ) );


  // Set the new bitmap transform
  RamMemWriteDoubleWord(0xffffff21UL); // bitmap transform
  RamMemWriteDoubleWord(x1 - minx);
  RamMemWriteDoubleWord(y1 - miny);
  RamMemWriteDoubleWord(x2 - minx);
  RamMemWriteDoubleWord(y2 - miny);
  RamMemWriteDoubleWord(x3 - minx);
  RamMemWriteDoubleWord(y3 - miny);

  RamMemWriteDoubleWord(bx1);
  RamMemWriteDoubleWord(by1);
  RamMemWriteDoubleWord(bx1);
  RamMemWriteDoubleWord(by3);
  RamMemWriteDoubleWord(bx3);
  RamMemWriteDoubleWord(by3);
  RamMemWriteDoubleWord(0);

  // Draw the quad
  RamMemWriteDoubleWord( VERTEX2F( PIXELS(minx), PIXELS(miny) ) );
}

void draw_faces(int dir)
{
    int R = 15;
    signed char *p = (signed char*)&CUBE_faces[0];
    unsigned char n;

//  GD.BlendFunc(ONE, ONE_MINUS_SRC_ALPHA);
//    GD.Begin(BITMAPS);
    RamMemWriteDoubleWord( BEGIN_BITMAPS );

    unsigned char i = 0;
    while ((n = (*p++)) != 0xff)
    {
        signed char nx = (p[0]);
        signed char ny = (p[1]);
        signed char nz = (p[2]);
        p += 3;

        unsigned char v1 = (p[0]);
        unsigned char v2 = (p[1]);
        unsigned char v3 = (p[2]);

        p += n;

        long x1 = projected[v1].x;
        long y1 = projected[v1].y;
        long x2 = projected[v2].x;
        long y2 = projected[v2].y;
        long x3 = projected[v3].x;
        long y3 = projected[v3].y;

        long area = (x1 - x3) * (y2 - y1) - (x1 - x2) * (y3 - y1);
        unsigned char face = (area < 0);

        if (face == dir)
        {
            unsigned short r = 80, g = 40, b = 0;  // Ambient

            if (face)
            {
                transform_normal( &nx, &ny, &nz);

                int d = max(0, -nz);                      // diffuse light from +ve Z

                r += 2 * d;
                g += 2 * d;
                b += 2 * d;
            }

            //r = constrain(r, 192, 255);
            //      GD.ColorRGB(min(255, r), min(255, g), min(255, b));
            //      GD.BitmapHandle(face);
            RamMemWriteDoubleWord( BITMAP_HANDLE(face) );

            RamMemWriteDoubleWord( USE_CELL(i) );

//            i = (i + 1) & 3;

            x1 >>= 4;
            y1 >>= 4;
            x2 >>= 4;
            y2 >>= 4;
            x3 >>= 4;
            y3 >>= 4;
            quad(x1, y1, x2, y2, x3, y3, 80 - 90, 64 - 90, 80 + 90, 64 + 90);
        }
    }
    RamMemWriteDoubleWord( DL_END );
}

/*****************************************************************/

/* simple trackball-like motion control */
/* Based on projtex.c - by David Yu and David Blythe, SGI */

float angle, axis[3] = {0,1,0};
float lastPos[3];

//void ptov(int x, int y, int width, int height, float v[3])
void ptov(int x, int y, int width, int height, float *v)
{
    float d, a;

    /* project x,y onto a hemi-sphere centered within width, height */
    v[0] = (2.0 * x - width) / width;
    v[1] = (2.0 * y - height) / height;
    d = sqrt(v[0] * v[0] + v[1] * v[1]);

    v[2] = cos((M_PI / 2.0) * ((d < 1.0) ? d : 1.0));
    a = 1.0 / sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

    v[0] *= a;
    v[1] *= a;
    v[2] *= a;
}

void startMotion(int x, int y)
{
    angle = 0.0;
//  ptov(x, y, SCREEN_WX, SCREEN_WY, lastPos);
    ptov(x, y, SCREEN_WX, SCREEN_WY, &lastPos[0]);
}

void trackMotion(int x, int y)
{
    float curPos[3], dx, dy, dz;

    ptov(x, y, SCREEN_WX, SCREEN_WY, curPos);

    dx = curPos[0] - lastPos[0];
    dy = curPos[1] - lastPos[1];
    dz = curPos[2] - lastPos[2];
    angle = (M_PI / 2) * sqrt(dx * dx + dy * dy + dz * dz);

    axis[0] = lastPos[1] * curPos[2] - lastPos[2] * curPos[1];
    axis[1] = lastPos[2] * curPos[0] - lastPos[0] * curPos[2];
    axis[2] = lastPos[0] * curPos[1] - lastPos[1] * curPos[0];

    float mag = 1 / sqrt(axis[0] * axis[0] + axis[1] * axis[1] + axis[2] * axis[2]);
    axis[0] *= mag;
    axis[1] *= mag;
    axis[2] *= mag;

    lastPos[0] = curPos[0];
    lastPos[1] = curPos[1];
    lastPos[2] = curPos[2];
}

/*****************************************************************/
#if 0

uint32_t f0;
int qq;

class MoviePlayer2
{
  uint16_t wp;
  Reader r;
  void loadsector() {
    byte buf[512];
    GD.__end();
    r.readsector(buf);
    GD.resume();
    GD.wr_n(0x0f0000UL + wp, buf, 512);
    wp += 512;
  }

public:
  int begin(const char *filename) {
    GD.__end();
    if (!r.openfile(filename)) {
      Serial.println("Open failed");
      return 0;
    }
    GD.resume();

uint32_t t0 = millis();
    wp = 0;
    while (wp < 0xfe00U)
      loadsector();
    uint32_t took = (millis() - t0);
Serial.println(took);
Serial.print(1000L * wp / took);
Serial.println(" bytes/s");

    GD.cmd_mediafifo(0x0f0000UL, 0x10000UL);
    GD.cmd_regwrite(REG_MEDIAFIFO_WRITE, wp);
GD.finish();

    if (0) {
      GD.cmd_playvideo(OPT_MEDIAFIFO);
    } else {
      GD.cmd_videostart();
    }

f0 = millis();
    return 1;
  }
  int service() {
    if (r.eof()) {
      return 0;
    } else {
      byte buf[512];

      uint16_t fullness = wp - GD.rd16(REG_MEDIAFIFO_READ);
      qq = 0;
      while (fullness < 0xfe00U) {
        loadsector();
        fullness += 512;
        qq += 512;
      }
      GD.wr16(REG_MEDIAFIFO_WRITE, wp);
      return 1;
    }
  }
};

MoviePlayer2 mp;

#endif
/*****************************************************************/

#define BG 25

//Bitmap background;

struct PanelTbl move;

void setup()
{
	JpegLoad( 1,table[1].source,    table[1].offset, table[1].size );
	ParamScreen.ImageWx=480;
	ParamScreen.ImageWy=272;
	ParamScreen.ImageOffset = table[1].offset;
	ParamScreen.dot_depth=2;

	ParamScreen.div_Wx = 1;
	ParamScreen.div_Wy = 80;

	ParamScreen.blkWx = ParamScreen.ImageWx / ParamScreen.div_Wx;
	ParamScreen.blkWy = ParamScreen.ImageWy / ParamScreen.div_Wy;
	ParamScreen.BGcolor = BLACK;
	ParamScreen.BGClear = (CLR_COL | CLR_STN | CLR_TAG);
	ParamScreen.Alpha = 0;

	move.x = 130;
	move.y = 56 ;
	move.wx = 220;
	move.wy = 160;
	move.offset = ParamScreen.ImageOffset;
	move.flag = fDispOn;

    RamCmdFlash();
    Trans( SetupStart, NULL, NULL );
    Trans( ScreenClean, NULL, NULL );
    Trans( TransStart, NULL, NULL );

    RamCmdFlash();
    Trans( SetupStart, NULL, NULL );
    Trans( ScreenClean, NULL, NULL );

    RamMemWriteDoubleWord( BITMAP_HANDLE(FACE_FRONT) );
    RamMemWriteDoubleWord( BITMAP_SIZE(BILINEAR, BORDER, BORDER, SCREEN_WX, SCREEN_WY) );

		RamMemWriteDoubleWord( BITMAP_LAYOUT(RGB565, ParamScreen.ImageWx*ParamScreen.dot_depth, ParamScreen.ImageWy) );

    RamMemWriteDoubleWord( BITMAP_SOURCE( RAM_G+ParamScreen.ImageOffset ) );

    RamMemWriteDoubleWord( BITMAP_HANDLE(FACE_BACK) );
    RamMemWriteDoubleWord( BITMAP_SIZE(NEAREST, BORDER, BORDER, SCREEN_WX, SCREEN_WY) );

		RamMemWriteDoubleWord( BITMAP_LAYOUT(RGB565, ParamScreen.ImageWx*ParamScreen.dot_depth, ParamScreen.ImageWy) );

    RamMemWriteDoubleWord( BITMAP_SOURCE( RAM_G+ParamScreen.ImageOffset ) );
    Trans( TransStart, NULL, NULL );

  startMotion(240, 136);
  trackMotion(247, 138);
}

char prev_touching = 0;

void loop( void )
{
//  unsigned long t0 = micros();

//  unsigned long took = micros() - t0;


    RamCmdFlash();
    Trans( SetupStart, NULL, NULL );
    Trans( ScreenClean, NULL, NULL );

//    GD.ColorRGB(48, 48, 90);
    // background.wallpaper();

//    GD.ColorRGB(255, 255, 255);

/*
    if (!prev_touching && GD.inputs.touching)
    {
        startMotion(GD.inputs.x, GD.inputs.y);
    }
    else if (GD.inputs.touching)
    {
       trackMotion(GD.inputs.x, GD.inputs.y);
    }
    prev_touching = GD.inputs.touching;
*/

    if (angle != 0.0f)
    rotation(angle, axis);

    project();
    draw_faces(FACE_BACK);
    draw_faces(FACE_FRONT);

    Trans( TransStart, NULL, NULL );
}



