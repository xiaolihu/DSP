
#define NDEC2TO1COEFS 40

//40-tap LPF for Factor = 2 decimation, not in normal order.
//polyphase-decimator
const short Dec2to1Coefs[NDEC2TO1COEFS] = {
             20,            279,             48,           -231,
           -134,            295,            275,           -335,
           -481,            327,            763,           -237,
          -1145,              6,           1695,            509,
          -2651,          -1872,           5610,          13631,
          13631,           5610,          -1872,          -2651,
            509,           1695,              6,          -1145,
           -237,            763,            327,           -481,
           -335,            275,            295,           -134,
           -231,             48,            279,             20 };

