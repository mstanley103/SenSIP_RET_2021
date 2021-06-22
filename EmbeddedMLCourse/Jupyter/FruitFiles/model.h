
    // !!! This file is generated using emlearn !!!

    #include <eml_trees.h>
    

EmlTreesNode model_nodes[25] = {
  { 1, 0.2976912260055542, 1, 10 },
  { 1, 0.2333003506064415, 2, 5 },
  { 2, 0.2052631601691246, 3, 4 },
  { -1, 1, -1, -1 },
  { -1, 2, -1, -1 },
  { 2, 0.24795082211494446, 6, 4 },
  { 0, 0.46919162571430206, 4, 7 },
  { 1, 0.2936105579137802, 3, 8 },
  { 2, 0.19989106804132462, 9, 3 },
  { -1, 0, -1, -1 },
  { 1, 0.36743415892124176, 11, 17 },
  { 2, 0.3045814484357834, 12, 23 },
  { 1, 0.3550420254468918, 13, 22 },
  { 2, 0.19204425811767578, 14, 15 },
  { 0, 0.5555230975151062, 3, 9 },
  { 0, 0.470058798789978, 16, 20 },
  { 2, 0.24735261499881744, 17, 18 },
  { -1, 3, -1, -1 },
  { 0, 0.4554065465927124, 9, 19 },
  { 0, 0.4578068256378174, 17, 9 },
  { 0, 0.5572599470615387, 9, 21 },
  { 0, 0.5575346648693085, 3, 9 },
  { 0, 0.4892857223749161, 17, 9 },
  { 0, 0.43092621862888336, 17, 24 },
  { 2, 0.36649659276008606, 9, 4 } 
};

int32_t model_tree_roots[1] = { 0 };

EmlTrees model = {
        25,
        model_nodes,	  
        1,
        model_tree_roots,
    };

static inline int32_t model_predict_tree_0(const float *features, int32_t features_length) {
          if (features[1] < 0.2976912260055542) {
              if (features[1] < 0.2333003506064415) {
                  if (features[2] < 0.2052631601691246) {
                      return 1;
                  } else {
                      return 2;
                  }
              } else {
                  if (features[2] < 0.24795082211494446) {
                      if (features[0] < 0.46919162571430206) {
                          return 2;
                      } else {
                          if (features[1] < 0.2936105579137802) {
                              return 1;
                          } else {
                              if (features[2] < 0.19989106804132462) {
                                  return 0;
                              } else {
                                  return 1;
                              }
                          }
                      }
                  } else {
                      return 2;
                  }
              }
          } else {
              if (features[1] < 0.36743415892124176) {
                  if (features[2] < 0.3045814484357834) {
                      if (features[1] < 0.3550420254468918) {
                          if (features[2] < 0.19204425811767578) {
                              if (features[0] < 0.5555230975151062) {
                                  return 1;
                              } else {
                                  return 0;
                              }
                          } else {
                              if (features[0] < 0.470058798789978) {
                                  if (features[2] < 0.24735261499881744) {
                                      return 3;
                                  } else {
                                      if (features[0] < 0.4554065465927124) {
                                          return 0;
                                      } else {
                                          if (features[0] < 0.4578068256378174) {
                                              return 3;
                                          } else {
                                              return 0;
                                          }
                                      }
                                  }
                              } else {
                                  if (features[0] < 0.5572599470615387) {
                                      return 0;
                                  } else {
                                      if (features[0] < 0.5575346648693085) {
                                          return 1;
                                      } else {
                                          return 0;
                                      }
                                  }
                              }
                          }
                      } else {
                          if (features[0] < 0.4892857223749161) {
                              return 3;
                          } else {
                              return 0;
                          }
                      }
                  } else {
                      if (features[0] < 0.43092621862888336) {
                          return 3;
                      } else {
                          if (features[2] < 0.36649659276008606) {
                              return 0;
                          } else {
                              return 2;
                          }
                      }
                  }
              } else {
                  return 3;
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
    