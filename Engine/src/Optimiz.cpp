static const float sin_tab[] = {
    0.000000, 0.012272, 0.024541, 0.036807,
    0.049068, 0.061321, 0.073565, 0.085797,
    0.098017, 0.110222, 0.122411, 0.134581,
    0.146730, 0.158858, 0.170962, 0.183040,
    0.195090, 0.207111, 0.219101, 0.231058,
    0.242980, 0.254866, 0.266713, 0.278520,
    0.290285, 0.302006, 0.313682, 0.325310,
    0.336890, 0.348419, 0.359895, 0.371317,
    0.382683, 0.393992, 0.405241, 0.416430,
    0.427555, 0.438616, 0.449611, 0.460539,
    0.471397, 0.482184, 0.492898, 0.503538,
    0.514103, 0.524590, 0.534998, 0.545325,
    0.555570, 0.565732, 0.575808, 0.585798,
    0.595699, 0.605511, 0.615232, 0.624860,
    0.634393, 0.643832, 0.653173, 0.662416,
    0.671559, 0.680601, 0.689541, 0.698376,
    0.707107, 0.715731, 0.724247, 0.732654,
    0.740951, 0.749136, 0.757209, 0.765167,
    0.773010, 0.780737, 0.788346, 0.795837,
    0.803208, 0.810457, 0.817585, 0.824589,
    0.831470, 0.838225, 0.844854, 0.851355,
    0.857729, 0.863973, 0.870087, 0.876070,
    0.881921, 0.887640, 0.893224, 0.898674,
    0.903989, 0.909168, 0.914210, 0.919114,
    0.923880, 0.928506, 0.932993, 0.937339,
    0.941544, 0.945607, 0.949528, 0.953306,
    0.956940, 0.960431, 0.963776, 0.966976,
    0.970031, 0.972940, 0.975702, 0.978317,
    0.980785, 0.983105, 0.985278, 0.987301,
    0.989177, 0.990903, 0.992480, 0.993907,
    0.995185, 0.996313, 0.997290, 0.998118,
    0.998795, 0.999322, 0.999699, 1.000000
};

//static const float PITWO = (3.14159265*2);

float fastSin(float x) {

    int idx = 0;
    if (x<0)
        idx = x*(-81.487332253);
    else
        idx = x*81.487332253; //512/(2*pi)
    idx %= 512;
    //int32_t idx2 = idx_untrimmed % 512;

    if (idx < 128)
        return sin_tab[idx];
    else if (idx < 256)
        return sin_tab[255 - idx];
    else if (idx < 384)
        return -sin_tab[idx - 256];
    else if (idx < 512)
        return -sin_tab[511 - idx];
    else return 0;
}

//Method using Log Base 2 Approximation
float fastSqrt(float x)
{
  union
  {
    int i;
    float x;
  } u;
  u.x = x;
  u.i = (1<<29) + (u.i >> 1) - (1<<22);
  return u.x;
}
