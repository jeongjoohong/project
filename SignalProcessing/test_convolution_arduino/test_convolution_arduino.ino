void setup () {
  Serial.begin(115200);

  // test sample values from matlab documatation
  //float u[] = {1, 0, 1};
  //float v[] = {2, 7};
  //float u[] = {1, 1, 1};
  //float v[] = {1, 1, 0, 0, 0, 1, 1};
  float u[] = {-1, 2, 3, -2, 0, 1, 2};
  float v[] = {2, 4, -1, 1};
  bool isSame = false;
  
  int uLength = sizeof(u) / sizeof(float);
  int vLength = sizeof(v) / sizeof(float);
  int wLength = isSame ? uLength : uLength + vLength - 1 ;
  float w[wLength] = {};

  convolution(u, uLength, v, vLength, w, isSame);
  for (int i = 0; i < wLength; i++) {
    Serial.println(w[i]);
  }
}

void loop() {
}

void convolution(float *u, int uLength, float *v, int vLength, float *w, bool isSame) {
  float tmpArr[uLength + vLength - 1] = {};
  for (int i = 0; i < uLength + vLength - 1; i++) {
    int index = i;
    float tmpVal = 0;
    for (int j = 0; j < vLength; j++) {
      if (index >= 0 && index < uLength) {
        tmpVal += u[index] * v[j];
      }
      index--;
      tmpArr[i] = tmpVal;
    }
  }
  if (isSame) {
    int start = ceil((vLength - 1) / 2.0);
    for (int i = 0; i < uLength; i++) {
      w[i] = tmpArr[start+i]; 
    }
  } else {
    for (int i = 0; i < uLength + vLength - 1; i++) {
      w[i] = tmpArr[i]; 
    }
  }
}
