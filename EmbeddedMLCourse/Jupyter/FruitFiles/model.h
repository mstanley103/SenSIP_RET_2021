
    // !!! This file is generated using emlearn !!!

    #include <eml_trees.h>
    

EmlTreesNode model_nodes[29] = {
  { 1, 0.3032273054122925, 1, 13 },
  { 1, 0.2431386336684227, 2, 7 },
  { 2, 0.20606061071157455, 3, 4 },
  { -1, 1, -1, -1 },
  { 1, 0.23446089029312134, 5, 6 },
  { -1, 2, -1, -1 },
  { 1, 0.23774703592061996, 3, 5 },
  { 2, 0.24795082211494446, 8, 5 },
  { 0, 0.5508620738983154, 9, 3 },
  { 0, 0.5489726066589355, 10, 12 },
  { 1, 0.29689162969589233, 3, 11 },
  { 2, 0.19978651404380798, 3, 12 },
  { -1, 0, -1, -1 },
  { 1, 0.35620300471782684, 14, 25 },
  { 2, 0.30425266921520233, 15, 24 },
  { 2, 0.19704487174749374, 16, 19 },
  { 1, 0.3164386451244354, 17, 12 },
  { 0, 0.5575346648693085, 18, 12 },
  { 0, 0.5519814789295197, 3, 3 },
  { 1, 0.35388994216918945, 20, 23 },
  { 1, 0.337279349565506, 12, 21 },
  { 1, 0.33742469549179077, 22, 12 },
  { -1, 3, -1, -1 },
  { 2, 0.24087435752153397, 12, 22 },
  { 2, 0.3840909004211426, 22, 5 },
  { 0, 0.49907834827899933, 26, 28 },
  { 2, 0.30628518760204315, 22, 27 },
  { 0, 0.3897174298763275, 22, 12 },
  { 0, 0.5298701524734497, 12, 5 } 
};

int32_t model_tree_roots[1] = { 0 };

EmlTrees model = {
        29,
        model_nodes,	  
        1,
        model_tree_roots,
    };

static inline int32_t model_predict_tree_0(const float *features, int32_t features_length) {
          if (features[1] < 0.3032273054122925) {
              if (features[1] < 0.2431386336684227) {
                  if (features[2] < 0.20606061071157455) {
                      return 1;
                  } else {
                      if (features[1] < 0.23446089029312134) {
                          return 2;
                      } else {
                          if (features[1] < 0.23774703592061996) {
                              return 1;
                          } else {
                              return 2;
                          }
                      }
                  }
              } else {
                  if (features[2] < 0.24795082211494446) {
                      if (features[0] < 0.5508620738983154) {
                          if (features[0] < 0.5489726066589355) {
                              if (features[1] < 0.29689162969589233) {
                                  return 1;
                              } else {
                                  if (features[2] < 0.19978651404380798) {
                                      return 1;
                                  } else {
                                      return 0;
                                  }
                              }
                          } else {
                              return 0;
                          }
                      } else {
                          return 1;
                      }
                  } else {
                      return 2;
                  }
              }
          } else {
              if (features[1] < 0.35620300471782684) {
                  if (features[2] < 0.30425266921520233) {
                      if (features[2] < 0.19704487174749374) {
                          if (features[1] < 0.3164386451244354) {
                              if (features[0] < 0.5575346648693085) {
                                  if (features[0] < 0.5519814789295197) {
                                      return 1;
                                  } else {
                                      return 1;
                                  }
                              } else {
                                  return 0;
                              }
                          } else {
                              return 0;
                          }
                      } else {
                          if (features[1] < 0.35388994216918945) {
                              if (features[1] < 0.337279349565506) {
                                  return 0;
                              } else {
                                  if (features[1] < 0.33742469549179077) {
                                      return 3;
                                  } else {
                                      return 0;
                                  }
                              }
                          } else {
                              if (features[2] < 0.24087435752153397) {
                                  return 0;
                              } else {
                                  return 3;
                              }
                          }
                      }
                  } else {
                      if (features[2] < 0.3840909004211426) {
                          return 3;
                      } else {
                          return 2;
                      }
                  }
              } else {
                  if (features[0] < 0.49907834827899933) {
                      if (features[2] < 0.30628518760204315) {
                          return 3;
                      } else {
                          if (features[0] < 0.3897174298763275) {
                              return 3;
                          } else {
                              return 0;
                          }
                      }
                  } else {
                      if (features[0] < 0.5298701524734497) {
                          return 0;
                      } else {
                          return 2;
                      }
                  }
              }
          }
        }
        

int32_t model_predict(const float *features, int32_t features_length) {

        int32_t votes[4] = {0,};
        int32_t _class = -1;

        _class = model_predict_tree_0(features, features_length); votes[_class] += 1;
    
        int32_t most_voted_class = -1;
        int32_t most_voted_votes = 0;
        for (int32_t i=0; i<4; i++) {

            if (votes[i] > most_voted_votes) {
                most_voted_class = i;
                most_voted_votes = votes[i];
            }
        }
        return most_voted_class;
    }
    