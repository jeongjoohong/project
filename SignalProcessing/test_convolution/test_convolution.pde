void setup() {
  // test sample values from matlab documatation
  //float[] u = {1, 0, 1};
  //float[] v = {2, 7};
  //float[] u = {1, 1, 1};
  //float[] v = {1, 1, 0, 0, 0, 1, 1};
  float[] u = { -1, 2, 3, -2, 0, 1, 2 };
  float[] v = { 2, 4, -1, 1 };
  float[] w = convolution(u, v, true);
  printArray(w);
}

float[] convolution(float[] u, float[] v, boolean isSame){
  float[] w = new float[u.length + v.length - 1];
  for (int i = 0; i < u.length+v.length-1; i++) {
    int index = i;
    float tmp = 0;
    for (int j = 0; j < v.length; j++) {
      if (index >= 0 && index < u.length) {
        tmp += u[index] * v[j];
        //print(tmp);
      }
      index--;
      w[i] = tmp;
    }
  }
  if (isSame) {
    float[] c = new float[u.length];
    int start = ceil((w.length - u.length) / 2.0);
    for (int i = 0; i < u.length; i++) {
      c[i] = w[start+i]; 
    }
    return c;
  } else {
    return w;
  }
}