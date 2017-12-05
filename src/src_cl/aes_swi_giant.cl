/*

    Single Work Item aes implementation

    Medium footprint
    Minimal t-tables used
    t-tables rotated on-the-spot

    WARNING WARNING WARNING: this took 135 hours and ~75GB of memory to compile

*/

#define MAX_EXKEY_SIZE_WORDS 64

#define NUM_WORDS 4

#define BLOCK_SIZE 16


__private uint Te0(uchar index) {
    switch (index) {
        case 0: return 0xc66363a5U;
        case 1: return 0xf87c7c84U;
        case 2: return 0xee777799U;
        case 3: return 0xf67b7b8dU;
        case 4: return 0xfff2f20dU;
        case 5: return 0xd66b6bbdU;
        case 6: return 0xde6f6fb1U;
        case 7: return 0x91c5c554U;
        case 8: return 0x60303050U;
        case 9: return 0x02010103U;
        case 10: return 0xce6767a9U;
        case 11: return 0x562b2b7dU;
        case 12: return 0xe7fefe19U;
        case 13: return 0xb5d7d762U;
        case 14: return 0x4dababe6U;
        case 15: return 0xec76769aU;
        case 16: return 0x8fcaca45U;
        case 17: return 0x1f82829dU;
        case 18: return 0x89c9c940U;
        case 19: return 0xfa7d7d87U;
        case 20: return 0xeffafa15U;
        case 21: return 0xb25959ebU;
        case 22: return 0x8e4747c9U;
        case 23: return 0xfbf0f00bU;
        case 24: return 0x41adadecU;
        case 25: return 0xb3d4d467U;
        case 26: return 0x5fa2a2fdU;
        case 27: return 0x45afafeaU;
        case 28: return 0x239c9cbfU;
        case 29: return 0x53a4a4f7U;
        case 30: return 0xe4727296U;
        case 31: return 0x9bc0c05bU;
        case 32: return 0x75b7b7c2U;
        case 33: return 0xe1fdfd1cU;
        case 34: return 0x3d9393aeU;
        case 35: return 0x4c26266aU;
        case 36: return 0x6c36365aU;
        case 37: return 0x7e3f3f41U;
        case 38: return 0xf5f7f702U;
        case 39: return 0x83cccc4fU;
        case 40: return 0x6834345cU;
        case 41: return 0x51a5a5f4U;
        case 42: return 0xd1e5e534U;
        case 43: return 0xf9f1f108U;
        case 44: return 0xe2717193U;
        case 45: return 0xabd8d873U;
        case 46: return 0x62313153U;
        case 47: return 0x2a15153fU;
        case 48: return 0x0804040cU;
        case 49: return 0x95c7c752U;
        case 50: return 0x46232365U;
        case 51: return 0x9dc3c35eU;
        case 52: return 0x30181828U;
        case 53: return 0x379696a1U;
        case 54: return 0x0a05050fU;
        case 55: return 0x2f9a9ab5U;
        case 56: return 0x0e070709U;
        case 57: return 0x24121236U;
        case 58: return 0x1b80809bU;
        case 59: return 0xdfe2e23dU;
        case 60: return 0xcdebeb26U;
        case 61: return 0x4e272769U;
        case 62: return 0x7fb2b2cdU;
        case 63: return 0xea75759fU;
        case 64: return 0x1209091bU;
        case 65: return 0x1d83839eU;
        case 66: return 0x582c2c74U;
        case 67: return 0x341a1a2eU;
        case 68: return 0x361b1b2dU;
        case 69: return 0xdc6e6eb2U;
        case 70: return 0xb45a5aeeU;
        case 71: return 0x5ba0a0fbU;
        case 72: return 0xa45252f6U;
        case 73: return 0x763b3b4dU;
        case 74: return 0xb7d6d661U;
        case 75: return 0x7db3b3ceU;
        case 76: return 0x5229297bU;
        case 77: return 0xdde3e33eU;
        case 78: return 0x5e2f2f71U;
        case 79: return 0x13848497U;
        case 80: return 0xa65353f5U;
        case 81: return 0xb9d1d168U;
        case 82: return 0x00000000U;
        case 83: return 0xc1eded2cU;
        case 84: return 0x40202060U;
        case 85: return 0xe3fcfc1fU;
        case 86: return 0x79b1b1c8U;
        case 87: return 0xb65b5bedU;
        case 88: return 0xd46a6abeU;
        case 89: return 0x8dcbcb46U;
        case 90: return 0x67bebed9U;
        case 91: return 0x7239394bU;
        case 92: return 0x944a4adeU;
        case 93: return 0x984c4cd4U;
        case 94: return 0xb05858e8U;
        case 95: return 0x85cfcf4aU;
        case 96: return 0xbbd0d06bU;
        case 97: return 0xc5efef2aU;
        case 98: return 0x4faaaae5U;
        case 99: return 0xedfbfb16U;
        case 100: return 0x864343c5U;
        case 101: return 0x9a4d4dd7U;
        case 102: return 0x66333355U;
        case 103: return 0x11858594U;
        case 104: return 0x8a4545cfU;
        case 105: return 0xe9f9f910U;
        case 106: return 0x04020206U;
        case 107: return 0xfe7f7f81U;
        case 108: return 0xa05050f0U;
        case 109: return 0x783c3c44U;
        case 110: return 0x259f9fbaU;
        case 111: return 0x4ba8a8e3U;
        case 112: return 0xa25151f3U;
        case 113: return 0x5da3a3feU;
        case 114: return 0x804040c0U;
        case 115: return 0x058f8f8aU;
        case 116: return 0x3f9292adU;
        case 117: return 0x219d9dbcU;
        case 118: return 0x70383848U;
        case 119: return 0xf1f5f504U;
        case 120: return 0x63bcbcdfU;
        case 121: return 0x77b6b6c1U;
        case 122: return 0xafdada75U;
        case 123: return 0x42212163U;
        case 124: return 0x20101030U;
        case 125: return 0xe5ffff1aU;
        case 126: return 0xfdf3f30eU;
        case 127: return 0xbfd2d26dU;
        case 128: return 0x81cdcd4cU;
        case 129: return 0x180c0c14U;
        case 130: return 0x26131335U;
        case 131: return 0xc3ecec2fU;
        case 132: return 0xbe5f5fe1U;
        case 133: return 0x359797a2U;
        case 134: return 0x884444ccU;
        case 135: return 0x2e171739U;
        case 136: return 0x93c4c457U;
        case 137: return 0x55a7a7f2U;
        case 138: return 0xfc7e7e82U;
        case 139: return 0x7a3d3d47U;
        case 140: return 0xc86464acU;
        case 141: return 0xba5d5de7U;
        case 142: return 0x3219192bU;
        case 143: return 0xe6737395U;
        case 144: return 0xc06060a0U;
        case 145: return 0x19818198U;
        case 146: return 0x9e4f4fd1U;
        case 147: return 0xa3dcdc7fU;
        case 148: return 0x44222266U;
        case 149: return 0x542a2a7eU;
        case 150: return 0x3b9090abU;
        case 151: return 0x0b888883U;
        case 152: return 0x8c4646caU;
        case 153: return 0xc7eeee29U;
        case 154: return 0x6bb8b8d3U;
        case 155: return 0x2814143cU;
        case 156: return 0xa7dede79U;
        case 157: return 0xbc5e5ee2U;
        case 158: return 0x160b0b1dU;
        case 159: return 0xaddbdb76U;
        case 160: return 0xdbe0e03bU;
        case 161: return 0x64323256U;
        case 162: return 0x743a3a4eU;
        case 163: return 0x140a0a1eU;
        case 164: return 0x924949dbU;
        case 165: return 0x0c06060aU;
        case 166: return 0x4824246cU;
        case 167: return 0xb85c5ce4U;
        case 168: return 0x9fc2c25dU;
        case 169: return 0xbdd3d36eU;
        case 170: return 0x43acacefU;
        case 171: return 0xc46262a6U;
        case 172: return 0x399191a8U;
        case 173: return 0x319595a4U;
        case 174: return 0xd3e4e437U;
        case 175: return 0xf279798bU;
        case 176: return 0xd5e7e732U;
        case 177: return 0x8bc8c843U;
        case 178: return 0x6e373759U;
        case 179: return 0xda6d6db7U;
        case 180: return 0x018d8d8cU;
        case 181: return 0xb1d5d564U;
        case 182: return 0x9c4e4ed2U;
        case 183: return 0x49a9a9e0U;
        case 184: return 0xd86c6cb4U;
        case 185: return 0xac5656faU;
        case 186: return 0xf3f4f407U;
        case 187: return 0xcfeaea25U;
        case 188: return 0xca6565afU;
        case 189: return 0xf47a7a8eU;
        case 190: return 0x47aeaee9U;
        case 191: return 0x10080818U;
        case 192: return 0x6fbabad5U;
        case 193: return 0xf0787888U;
        case 194: return 0x4a25256fU;
        case 195: return 0x5c2e2e72U;
        case 196: return 0x381c1c24U;
        case 197: return 0x57a6a6f1U;
        case 198: return 0x73b4b4c7U;
        case 199: return 0x97c6c651U;
        case 200: return 0xcbe8e823U;
        case 201: return 0xa1dddd7cU;
        case 202: return 0xe874749cU;
        case 203: return 0x3e1f1f21U;
        case 204: return 0x964b4bddU;
        case 205: return 0x61bdbddcU;
        case 206: return 0x0d8b8b86U;
        case 207: return 0x0f8a8a85U;
        case 208: return 0xe0707090U;
        case 209: return 0x7c3e3e42U;
        case 210: return 0x71b5b5c4U;
        case 211: return 0xcc6666aaU;
        case 212: return 0x904848d8U;
        case 213: return 0x06030305U;
        case 214: return 0xf7f6f601U;
        case 215: return 0x1c0e0e12U;
        case 216: return 0xc26161a3U;
        case 217: return 0x6a35355fU;
        case 218: return 0xae5757f9U;
        case 219: return 0x69b9b9d0U;
        case 220: return 0x17868691U;
        case 221: return 0x99c1c158U;
        case 222: return 0x3a1d1d27U;
        case 223: return 0x279e9eb9U;
        case 224: return 0xd9e1e138U;
        case 225: return 0xebf8f813U;
        case 226: return 0x2b9898b3U;
        case 227: return 0x22111133U;
        case 228: return 0xd26969bbU;
        case 229: return 0xa9d9d970U;
        case 230: return 0x078e8e89U;
        case 231: return 0x339494a7U;
        case 232: return 0x2d9b9bb6U;
        case 233: return 0x3c1e1e22U;
        case 234: return 0x15878792U;
        case 235: return 0xc9e9e920U;
        case 236: return 0x87cece49U;
        case 237: return 0xaa5555ffU;
        case 238: return 0x50282878U;
        case 239: return 0xa5dfdf7aU;
        case 240: return 0x038c8c8fU;
        case 241: return 0x59a1a1f8U;
        case 242: return 0x09898980U;
        case 243: return 0x1a0d0d17U;
        case 244: return 0x65bfbfdaU;
        case 245: return 0xd7e6e631U;
        case 246: return 0x844242c6U;
        case 247: return 0xd06868b8U;
        case 248: return 0x824141c3U;
        case 249: return 0x299999b0U;
        case 250: return 0x5a2d2d77U;
        case 251: return 0x1e0f0f11U;
        case 252: return 0x7bb0b0cbU;
        case 253: return 0xa85454fcU;
        case 254: return 0x6dbbbbd6U;
        case 255: return 0x2c16163aU;
    }
}

__private uint Td0(uchar index) {
    switch (index) {
        case 0: return 0x51f4a750U;
        case 1: return 0x7e416553U;
        case 2: return 0x1a17a4c3U;
        case 3: return 0x3a275e96U;
        case 4: return 0x3bab6bcbU;
        case 5: return 0x1f9d45f1U;
        case 6: return 0xacfa58abU;
        case 7: return 0x4be30393U;
        case 8: return 0x2030fa55U;
        case 9: return 0xad766df6U;
        case 10: return 0x88cc7691U;
        case 11: return 0xf5024c25U;
        case 12: return 0x4fe5d7fcU;
        case 13: return 0xc52acbd7U;
        case 14: return 0x26354480U;
        case 15: return 0xb562a38fU;
        case 16: return 0xdeb15a49U;
        case 17: return 0x25ba1b67U;
        case 18: return 0x45ea0e98U;
        case 19: return 0x5dfec0e1U;
        case 20: return 0xc32f7502U;
        case 21: return 0x814cf012U;
        case 22: return 0x8d4697a3U;
        case 23: return 0x6bd3f9c6U;
        case 24: return 0x038f5fe7U;
        case 25: return 0x15929c95U;
        case 26: return 0xbf6d7aebU;
        case 27: return 0x955259daU;
        case 28: return 0xd4be832dU;
        case 29: return 0x587421d3U;
        case 30: return 0x49e06929U;
        case 31: return 0x8ec9c844U;
        case 32: return 0x75c2896aU;
        case 33: return 0xf48e7978U;
        case 34: return 0x99583e6bU;
        case 35: return 0x27b971ddU;
        case 36: return 0xbee14fb6U;
        case 37: return 0xf088ad17U;
        case 38: return 0xc920ac66U;
        case 39: return 0x7dce3ab4U;
        case 40: return 0x63df4a18U;
        case 41: return 0xe51a3182U;
        case 42: return 0x97513360U;
        case 43: return 0x62537f45U;
        case 44: return 0xb16477e0U;
        case 45: return 0xbb6bae84U;
        case 46: return 0xfe81a01cU;
        case 47: return 0xf9082b94U;
        case 48: return 0x70486858U;
        case 49: return 0x8f45fd19U;
        case 50: return 0x94de6c87U;
        case 51: return 0x527bf8b7U;
        case 52: return 0xab73d323U;
        case 53: return 0x724b02e2U;
        case 54: return 0xe31f8f57U;
        case 55: return 0x6655ab2aU;
        case 56: return 0xb2eb2807U;
        case 57: return 0x2fb5c203U;
        case 58: return 0x86c57b9aU;
        case 59: return 0xd33708a5U;
        case 60: return 0x302887f2U;
        case 61: return 0x23bfa5b2U;
        case 62: return 0x02036abaU;
        case 63: return 0xed16825cU;
        case 64: return 0x8acf1c2bU;
        case 65: return 0xa779b492U;
        case 66: return 0xf307f2f0U;
        case 67: return 0x4e69e2a1U;
        case 68: return 0x65daf4cdU;
        case 69: return 0x0605bed5U;
        case 70: return 0xd134621fU;
        case 71: return 0xc4a6fe8aU;
        case 72: return 0x342e539dU;
        case 73: return 0xa2f355a0U;
        case 74: return 0x058ae132U;
        case 75: return 0xa4f6eb75U;
        case 76: return 0x0b83ec39U;
        case 77: return 0x4060efaaU;
        case 78: return 0x5e719f06U;
        case 79: return 0xbd6e1051U;
        case 80: return 0x3e218af9U;
        case 81: return 0x96dd063dU;
        case 82: return 0xdd3e05aeU;
        case 83: return 0x4de6bd46U;
        case 84: return 0x91548db5U;
        case 85: return 0x71c45d05U;
        case 86: return 0x0406d46fU;
        case 87: return 0x605015ffU;
        case 88: return 0x1998fb24U;
        case 89: return 0xd6bde997U;
        case 90: return 0x894043ccU;
        case 91: return 0x67d99e77U;
        case 92: return 0xb0e842bdU;
        case 93: return 0x07898b88U;
        case 94: return 0xe7195b38U;
        case 95: return 0x79c8eedbU;
        case 96: return 0xa17c0a47U;
        case 97: return 0x7c420fe9U;
        case 98: return 0xf8841ec9U;
        case 99: return 0x00000000U;
        case 100: return 0x09808683U;
        case 101: return 0x322bed48U;
        case 102: return 0x1e1170acU;
        case 103: return 0x6c5a724eU;
        case 104: return 0xfd0efffbU;
        case 105: return 0x0f853856U;
        case 106: return 0x3daed51eU;
        case 107: return 0x362d3927U;
        case 108: return 0x0a0fd964U;
        case 109: return 0x685ca621U;
        case 110: return 0x9b5b54d1U;
        case 111: return 0x24362e3aU;
        case 112: return 0x0c0a67b1U;
        case 113: return 0x9357e70fU;
        case 114: return 0xb4ee96d2U;
        case 115: return 0x1b9b919eU;
        case 116: return 0x80c0c54fU;
        case 117: return 0x61dc20a2U;
        case 118: return 0x5a774b69U;
        case 119: return 0x1c121a16U;
        case 120: return 0xe293ba0aU;
        case 121: return 0xc0a02ae5U;
        case 122: return 0x3c22e043U;
        case 123: return 0x121b171dU;
        case 124: return 0x0e090d0bU;
        case 125: return 0xf28bc7adU;
        case 126: return 0x2db6a8b9U;
        case 127: return 0x141ea9c8U;
        case 128: return 0x57f11985U;
        case 129: return 0xaf75074cU;
        case 130: return 0xee99ddbbU;
        case 131: return 0xa37f60fdU;
        case 132: return 0xf701269fU;
        case 133: return 0x5c72f5bcU;
        case 134: return 0x44663bc5U;
        case 135: return 0x5bfb7e34U;
        case 136: return 0x8b432976U;
        case 137: return 0xcb23c6dcU;
        case 138: return 0xb6edfc68U;
        case 139: return 0xb8e4f163U;
        case 140: return 0xd731dccaU;
        case 141: return 0x42638510U;
        case 142: return 0x13972240U;
        case 143: return 0x84c61120U;
        case 144: return 0x854a247dU;
        case 145: return 0xd2bb3df8U;
        case 146: return 0xaef93211U;
        case 147: return 0xc729a16dU;
        case 148: return 0x1d9e2f4bU;
        case 149: return 0xdcb230f3U;
        case 150: return 0x0d8652ecU;
        case 151: return 0x77c1e3d0U;
        case 152: return 0x2bb3166cU;
        case 153: return 0xa970b999U;
        case 154: return 0x119448faU;
        case 155: return 0x47e96422U;
        case 156: return 0xa8fc8cc4U;
        case 157: return 0xa0f03f1aU;
        case 158: return 0x567d2cd8U;
        case 159: return 0x223390efU;
        case 160: return 0x87494ec7U;
        case 161: return 0xd938d1c1U;
        case 162: return 0x8ccaa2feU;
        case 163: return 0x98d40b36U;
        case 164: return 0xa6f581cfU;
        case 165: return 0xa57ade28U;
        case 166: return 0xdab78e26U;
        case 167: return 0x3fadbfa4U;
        case 168: return 0x2c3a9de4U;
        case 169: return 0x5078920dU;
        case 170: return 0x6a5fcc9bU;
        case 171: return 0x547e4662U;
        case 172: return 0xf68d13c2U;
        case 173: return 0x90d8b8e8U;
        case 174: return 0x2e39f75eU;
        case 175: return 0x82c3aff5U;
        case 176: return 0x9f5d80beU;
        case 177: return 0x69d0937cU;
        case 178: return 0x6fd52da9U;
        case 179: return 0xcf2512b3U;
        case 180: return 0xc8ac993bU;
        case 181: return 0x10187da7U;
        case 182: return 0xe89c636eU;
        case 183: return 0xdb3bbb7bU;
        case 184: return 0xcd267809U;
        case 185: return 0x6e5918f4U;
        case 186: return 0xec9ab701U;
        case 187: return 0x834f9aa8U;
        case 188: return 0xe6956e65U;
        case 189: return 0xaaffe67eU;
        case 190: return 0x21bccf08U;
        case 191: return 0xef15e8e6U;
        case 192: return 0xbae79bd9U;
        case 193: return 0x4a6f36ceU;
        case 194: return 0xea9f09d4U;
        case 195: return 0x29b07cd6U;
        case 196: return 0x31a4b2afU;
        case 197: return 0x2a3f2331U;
        case 198: return 0xc6a59430U;
        case 199: return 0x35a266c0U;
        case 200: return 0x744ebc37U;
        case 201: return 0xfc82caa6U;
        case 202: return 0xe090d0b0U;
        case 203: return 0x33a7d815U;
        case 204: return 0xf104984aU;
        case 205: return 0x41ecdaf7U;
        case 206: return 0x7fcd500eU;
        case 207: return 0x1791f62fU;
        case 208: return 0x764dd68dU;
        case 209: return 0x43efb04dU;
        case 210: return 0xccaa4d54U;
        case 211: return 0xe49604dfU;
        case 212: return 0x9ed1b5e3U;
        case 213: return 0x4c6a881bU;
        case 214: return 0xc12c1fb8U;
        case 215: return 0x4665517fU;
        case 216: return 0x9d5eea04U;
        case 217: return 0x018c355dU;
        case 218: return 0xfa877473U;
        case 219: return 0xfb0b412eU;
        case 220: return 0xb3671d5aU;
        case 221: return 0x92dbd252U;
        case 222: return 0xe9105633U;
        case 223: return 0x6dd64713U;
        case 224: return 0x9ad7618cU;
        case 225: return 0x37a10c7aU;
        case 226: return 0x59f8148eU;
        case 227: return 0xeb133c89U;
        case 228: return 0xcea927eeU;
        case 229: return 0xb761c935U;
        case 230: return 0xe11ce5edU;
        case 231: return 0x7a47b13cU;
        case 232: return 0x9cd2df59U;
        case 233: return 0x55f2733fU;
        case 234: return 0x1814ce79U;
        case 235: return 0x73c737bfU;
        case 236: return 0x53f7cdeaU;
        case 237: return 0x5ffdaa5bU;
        case 238: return 0xdf3d6f14U;
        case 239: return 0x7844db86U;
        case 240: return 0xcaaff381U;
        case 241: return 0xb968c43eU;
        case 242: return 0x3824342cU;
        case 243: return 0xc2a3405fU;
        case 244: return 0x161dc372U;
        case 245: return 0xbce2250cU;
        case 246: return 0x283c498bU;
        case 247: return 0xff0d9541U;
        case 248: return 0x39a80171U;
        case 249: return 0x080cb3deU;
        case 250: return 0xd8b4e49cU;
        case 251: return 0x6456c190U;
        case 252: return 0x7bcb8461U;
        case 253: return 0xd532b670U;
        case 254: return 0x486c5c74U;
        case 255: return 0xd0b85742U;
    }
}

__private uchar Td4(uchar index) {
    switch (index) {
        case 0: return 0x52U;
        case 1: return 0x09U;
        case 2: return 0x6aU;
        case 3: return 0xd5U;
        case 4: return 0x30U;
        case 5: return 0x36U;
        case 6: return 0xa5U;
        case 7: return 0x38U;
        case 8: return 0xbfU;
        case 9: return 0x40U;
        case 10: return 0xa3U;
        case 11: return 0x9eU;
        case 12: return 0x81U;
        case 13: return 0xf3U;
        case 14: return 0xd7U;
        case 15: return 0xfbU;
        case 16: return 0x7cU;
        case 17: return 0xe3U;
        case 18: return 0x39U;
        case 19: return 0x82U;
        case 20: return 0x9bU;
        case 21: return 0x2fU;
        case 22: return 0xffU;
        case 23: return 0x87U;
        case 24: return 0x34U;
        case 25: return 0x8eU;
        case 26: return 0x43U;
        case 27: return 0x44U;
        case 28: return 0xc4U;
        case 29: return 0xdeU;
        case 30: return 0xe9U;
        case 31: return 0xcbU;
        case 32: return 0x54U;
        case 33: return 0x7bU;
        case 34: return 0x94U;
        case 35: return 0x32U;
        case 36: return 0xa6U;
        case 37: return 0xc2U;
        case 38: return 0x23U;
        case 39: return 0x3dU;
        case 40: return 0xeeU;
        case 41: return 0x4cU;
        case 42: return 0x95U;
        case 43: return 0x0bU;
        case 44: return 0x42U;
        case 45: return 0xfaU;
        case 46: return 0xc3U;
        case 47: return 0x4eU;
        case 48: return 0x08U;
        case 49: return 0x2eU;
        case 50: return 0xa1U;
        case 51: return 0x66U;
        case 52: return 0x28U;
        case 53: return 0xd9U;
        case 54: return 0x24U;
        case 55: return 0xb2U;
        case 56: return 0x76U;
        case 57: return 0x5bU;
        case 58: return 0xa2U;
        case 59: return 0x49U;
        case 60: return 0x6dU;
        case 61: return 0x8bU;
        case 62: return 0xd1U;
        case 63: return 0x25U;
        case 64: return 0x72U;
        case 65: return 0xf8U;
        case 66: return 0xf6U;
        case 67: return 0x64U;
        case 68: return 0x86U;
        case 69: return 0x68U;
        case 70: return 0x98U;
        case 71: return 0x16U;
        case 72: return 0xd4U;
        case 73: return 0xa4U;
        case 74: return 0x5cU;
        case 75: return 0xccU;
        case 76: return 0x5dU;
        case 77: return 0x65U;
        case 78: return 0xb6U;
        case 79: return 0x92U;
        case 80: return 0x6cU;
        case 81: return 0x70U;
        case 82: return 0x48U;
        case 83: return 0x50U;
        case 84: return 0xfdU;
        case 85: return 0xedU;
        case 86: return 0xb9U;
        case 87: return 0xdaU;
        case 88: return 0x5eU;
        case 89: return 0x15U;
        case 90: return 0x46U;
        case 91: return 0x57U;
        case 92: return 0xa7U;
        case 93: return 0x8dU;
        case 94: return 0x9dU;
        case 95: return 0x84U;
        case 96: return 0x90U;
        case 97: return 0xd8U;
        case 98: return 0xabU;
        case 99: return 0x00U;
        case 100: return 0x8cU;
        case 101: return 0xbcU;
        case 102: return 0xd3U;
        case 103: return 0x0aU;
        case 104: return 0xf7U;
        case 105: return 0xe4U;
        case 106: return 0x58U;
        case 107: return 0x05U;
        case 108: return 0xb8U;
        case 109: return 0xb3U;
        case 110: return 0x45U;
        case 111: return 0x06U;
        case 112: return 0xd0U;
        case 113: return 0x2cU;
        case 114: return 0x1eU;
        case 115: return 0x8fU;
        case 116: return 0xcaU;
        case 117: return 0x3fU;
        case 118: return 0x0fU;
        case 119: return 0x02U;
        case 120: return 0xc1U;
        case 121: return 0xafU;
        case 122: return 0xbdU;
        case 123: return 0x03U;
        case 124: return 0x01U;
        case 125: return 0x13U;
        case 126: return 0x8aU;
        case 127: return 0x6bU;
        case 128: return 0x3aU;
        case 129: return 0x91U;
        case 130: return 0x11U;
        case 131: return 0x41U;
        case 132: return 0x4fU;
        case 133: return 0x67U;
        case 134: return 0xdcU;
        case 135: return 0xeaU;
        case 136: return 0x97U;
        case 137: return 0xf2U;
        case 138: return 0xcfU;
        case 139: return 0xceU;
        case 140: return 0xf0U;
        case 141: return 0xb4U;
        case 142: return 0xe6U;
        case 143: return 0x73U;
        case 144: return 0x96U;
        case 145: return 0xacU;
        case 146: return 0x74U;
        case 147: return 0x22U;
        case 148: return 0xe7U;
        case 149: return 0xadU;
        case 150: return 0x35U;
        case 151: return 0x85U;
        case 152: return 0xe2U;
        case 153: return 0xf9U;
        case 154: return 0x37U;
        case 155: return 0xe8U;
        case 156: return 0x1cU;
        case 157: return 0x75U;
        case 158: return 0xdfU;
        case 159: return 0x6eU;
        case 160: return 0x47U;
        case 161: return 0xf1U;
        case 162: return 0x1aU;
        case 163: return 0x71U;
        case 164: return 0x1dU;
        case 165: return 0x29U;
        case 166: return 0xc5U;
        case 167: return 0x89U;
        case 168: return 0x6fU;
        case 169: return 0xb7U;
        case 170: return 0x62U;
        case 171: return 0x0eU;
        case 172: return 0xaaU;
        case 173: return 0x18U;
        case 174: return 0xbeU;
        case 175: return 0x1bU;
        case 176: return 0xfcU;
        case 177: return 0x56U;
        case 178: return 0x3eU;
        case 179: return 0x4bU;
        case 180: return 0xc6U;
        case 181: return 0xd2U;
        case 182: return 0x79U;
        case 183: return 0x20U;
        case 184: return 0x9aU;
        case 185: return 0xdbU;
        case 186: return 0xc0U;
        case 187: return 0xfeU;
        case 188: return 0x78U;
        case 189: return 0xcdU;
        case 190: return 0x5aU;
        case 191: return 0xf4U;
        case 192: return 0x1fU;
        case 193: return 0xddU;
        case 194: return 0xa8U;
        case 195: return 0x33U;
        case 196: return 0x88U;
        case 197: return 0x07U;
        case 198: return 0xc7U;
        case 199: return 0x31U;
        case 200: return 0xb1U;
        case 201: return 0x12U;
        case 202: return 0x10U;
        case 203: return 0x59U;
        case 204: return 0x27U;
        case 205: return 0x80U;
        case 206: return 0xecU;
        case 207: return 0x5fU;
        case 208: return 0x60U;
        case 209: return 0x51U;
        case 210: return 0x7fU;
        case 211: return 0xa9U;
        case 212: return 0x19U;
        case 213: return 0xb5U;
        case 214: return 0x4aU;
        case 215: return 0x0dU;
        case 216: return 0x2dU;
        case 217: return 0xe5U;
        case 218: return 0x7aU;
        case 219: return 0x9fU;
        case 220: return 0x93U;
        case 221: return 0xc9U;
        case 222: return 0x9cU;
        case 223: return 0xefU;
        case 224: return 0xa0U;
        case 225: return 0xe0U;
        case 226: return 0x3bU;
        case 227: return 0x4dU;
        case 228: return 0xaeU;
        case 229: return 0x2aU;
        case 230: return 0xf5U;
        case 231: return 0xb0U;
        case 232: return 0xc8U;
        case 233: return 0xebU;
        case 234: return 0xbbU;
        case 235: return 0x3cU;
        case 236: return 0x83U;
        case 237: return 0x53U;
        case 238: return 0x99U;
        case 239: return 0x61U;
        case 240: return 0x17U;
        case 241: return 0x2bU;
        case 242: return 0x04U;
        case 243: return 0x7eU;
        case 244: return 0xbaU;
        case 245: return 0x77U;
        case 246: return 0xd6U;
        case 247: return 0x26U;
        case 248: return 0xe1U;
        case 249: return 0x69U;
        case 250: return 0x14U;
        case 251: return 0x63U;
        case 252: return 0x55U;
        case 253: return 0x21U;
        case 254: return 0x0cU;
        case 255: return 0x7dU;
    }
}

#define ROTATEU32(n) (((n) >> 8) | ((n) << 24))

#define Te1(i) ROTATEU32(Te0(i))
#define Te2(i) ROTATEU32(Te1(i))
#define Te3(i) ROTATEU32(Te2(i))

#define Td1(i) ROTATEU32(Td0(i))
#define Td2(i) ROTATEU32(Td1(i))
#define Td3(i) ROTATEU32(Td2(i))


#define AES_KEY_INDEPENDENT_ENC_ROUND(s, t)                                                                         \
{                                                                                                                   \
    (t)[0] = Te0((s)[0] >> 24) ^ Te1(((s)[1] >> 16) & 0xff) ^ Te2(((s)[2] >>  8) & 0xff) ^ Te3((s)[3] & 0xff);      \
    (t)[1] = Te0((s)[1] >> 24) ^ Te1(((s)[2] >> 16) & 0xff) ^ Te2(((s)[3] >>  8) & 0xff) ^ Te3((s)[0] & 0xff);      \
    (t)[2] = Te0((s)[2] >> 24) ^ Te1(((s)[3] >> 16) & 0xff) ^ Te2(((s)[0] >>  8) & 0xff) ^ Te3((s)[1] & 0xff);      \
    (t)[3] = Te0((s)[3] >> 24) ^ Te1(((s)[0] >> 16) & 0xff) ^ Te2(((s)[1] >>  8) & 0xff) ^ Te3((s)[2] & 0xff);      \
}

#define AES_KEY_INDEPENDENT_ENC_ROUND_FINAL(s, t)                              \
{                                                                              \
    (t)[0] = (Te2( (s)[0] >> 24)         & 0xff000000) ^                       \
             (Te3(((s)[1] >> 16) & 0xff) & 0x00ff0000) ^                       \
             (Te0(((s)[2] >>  8) & 0xff) & 0x0000ff00) ^                       \
             (Te1( (s)[3]        & 0xff) & 0x000000ff);                        \
    (t)[1] = (Te2( (s)[1] >> 24)         & 0xff000000) ^                       \
             (Te3(((s)[2] >> 16) & 0xff) & 0x00ff0000) ^                       \
             (Te0(((s)[3] >>  8) & 0xff) & 0x0000ff00) ^                       \
             (Te1( (s)[0]        & 0xff) & 0x000000ff);                        \
    (t)[2] = (Te2( (s)[2] >> 24)         & 0xff000000) ^                       \
             (Te3(((s)[3] >> 16) & 0xff) & 0x00ff0000) ^                       \
             (Te0(((s)[0] >>  8) & 0xff) & 0x0000ff00) ^                       \
             (Te1( (s)[1]        & 0xff) & 0x000000ff);                        \
    (t)[3] = (Te2( (s)[3] >> 24)         & 0xff000000) ^                       \
             (Te3(((s)[0] >> 16) & 0xff) & 0x00ff0000) ^                       \
             (Te0(((s)[1] >>  8) & 0xff) & 0x0000ff00) ^                       \
             (Te1( (s)[2]        & 0xff) & 0x000000ff);                        \
}

#define AES_KEY_INDEPENDENT_DEC_ROUND(s, t)                                                                         \
{                                                                                                                   \
    (t)[0] = Td0((s)[0] >> 24) ^ Td1(((s)[3] >> 16) & 0xff) ^ Td2(((s)[2] >>  8) & 0xff) ^ Td3((s)[1] & 0xff);      \
    (t)[1] = Td0((s)[1] >> 24) ^ Td1(((s)[0] >> 16) & 0xff) ^ Td2(((s)[3] >>  8) & 0xff) ^ Td3((s)[2] & 0xff);      \
    (t)[2] = Td0((s)[2] >> 24) ^ Td1(((s)[1] >> 16) & 0xff) ^ Td2(((s)[0] >>  8) & 0xff) ^ Td3((s)[3] & 0xff);      \
    (t)[3] = Td0((s)[3] >> 24) ^ Td1(((s)[2] >> 16) & 0xff) ^ Td2(((s)[1] >>  8) & 0xff) ^ Td3((s)[0] & 0xff);      \
}

#define AES_KEY_INDEPENDENT_DEC_ROUND_FINAL(s, t)                              \
{                                                                              \
    (t)[0] = (Td4( (s)[0] >> 24)         << 24) ^                              \
             (Td4(((s)[3] >> 16) & 0xff) << 16) ^                              \
             (Td4(((s)[2] >>  8) & 0xff) <<  8) ^                              \
             (Td4( (s)[1]        & 0xff)      );                               \
    (t)[1] = (Td4( (s)[1] >> 24)         << 24) ^                              \
             (Td4(((s)[0] >> 16) & 0xff) << 16) ^                              \
             (Td4(((s)[3] >>  8) & 0xff) <<  8) ^                              \
             (Td4( (s)[2]        & 0xff)      );                               \
    (t)[2] = (Td4( (s)[2] >> 24)         << 24) ^                              \
             (Td4(((s)[1] >> 16) & 0xff) << 16) ^                              \
             (Td4(((s)[0] >>  8) & 0xff) <<  8) ^                              \
             (Td4( (s)[3]        & 0xff)      );                               \
    (t)[3] = (Td4( (s)[3] >> 24)         << 24) ^                              \
             (Td4(((s)[2] >> 16) & 0xff) << 16) ^                              \
             (Td4(((s)[1] >>  8) & 0xff) <<  8) ^                              \
             (Td4( (s)[0]        & 0xff)      );                               \
}

#define GETU32(byte_array)                                                                                                  \
    ((uint)byte_array[3]) + (((uint)byte_array[2]) << 8) + (((uint)byte_array[1]) << 16) + (((uint)byte_array[0]) << 24)

#define PUT32(byte_array, word32)                                       \
{                                                                       \
    byte_array[3] = (uchar)(word32 & 0xff);                             \
    byte_array[2] = (uchar)((word32 >> 8) & 0xff);                      \
    byte_array[1] = (uchar)((word32 >> 16) & 0xff);                     \
    byte_array[0] = (uchar)((word32 >> 24) & 0xff);                     \
}


void add_round_key(__private uint* state_in,
                   __local uint* w,
                   __private uint* state_out,
                   __private size_t i) {
    #pragma unroll
    for (size_t j = 0; j < NUM_WORDS; ++j) {
        state_out[j] = state_in[j] ^ w[i + j];
    }
}

void finalize_inverted_key(__local uint* w, unsigned int num_rounds) {
    for (size_t i = 1; i < num_rounds; i++) {
        __local uint* rk = w + (i*4);
        rk[0] =
            Td0(Te1((rk[0] >> 24)       ) & 0xff) ^
            Td1(Te1((rk[0] >> 16) & 0xff) & 0xff) ^
            Td2(Te1((rk[0] >>  8) & 0xff) & 0xff) ^
            Td3(Te1((rk[0]      ) & 0xff) & 0xff);
        rk[1] =
            Td0(Te1((rk[1] >> 24)       ) & 0xff) ^
            Td1(Te1((rk[1] >> 16) & 0xff) & 0xff) ^
            Td2(Te1((rk[1] >>  8) & 0xff) & 0xff) ^
            Td3(Te1((rk[1]      ) & 0xff) & 0xff);
        rk[2] =
            Td0(Te1((rk[2] >> 24)       ) & 0xff) ^
            Td1(Te1((rk[2] >> 16) & 0xff) & 0xff) ^
            Td2(Te1((rk[2] >>  8) & 0xff) & 0xff) ^
            Td3(Te1((rk[2]      ) & 0xff) & 0xff);
        rk[3] =
            Td0(Te1((rk[3] >> 24)       ) & 0xff) ^
            Td1(Te1((rk[3] >> 16) & 0xff) & 0xff) ^
            Td2(Te1((rk[3] >>  8) & 0xff) & 0xff) ^
            Td3(Te1((rk[3] ) & 0xff) & 0xff);
    }
}


#define INNER_AES_LOOP(step1, step2)                                            \
{                                                                               \
    size_t r = 1;                                                               \
    _Pragma("unroll")                                                           \
    for (size_t c = 0; c < 4; c++) {                                            \
        step1;                                                                  \
        step2;                                                                  \
        ++r;                                                                    \
        step1;                                                                  \
        step2;                                                                  \
        ++r;                                                                    \
    }                                                                           \
    if (num_rounds >= 12) {                                                     \
        step1;                                                                  \
        step2;                                                                  \
        ++r;                                                                    \
        step1;                                                                  \
        step2;                                                                  \
        ++r;                                                                    \
    }                                                                           \
    if (num_rounds >= 14) {                                                     \
        step1;                                                                  \
        step2;                                                                  \
        ++r;                                                                    \
        step1;                                                                  \
        step2;                                                                  \
        ++r;                                                                    \
    }                                                                           \
    step1;                                                                      \
    step2;                                                                      \
}


void encrypt(__private uchar state_in[BLOCK_SIZE],
             __local uint* w,
             __private uchar state_out[BLOCK_SIZE],
             unsigned int num_rounds) {

    uint temp_state1[NUM_WORDS], temp_state2[NUM_WORDS];

    #pragma unroll
    for (size_t chunk=0; chunk<NUM_WORDS; chunk++) {
        temp_state1[chunk] = GETU32((state_in + (chunk << 2)));
    }

    add_round_key(temp_state1, w, temp_state2, 0);
    INNER_AES_LOOP(
                   AES_KEY_INDEPENDENT_ENC_ROUND(temp_state2, temp_state1),
                   add_round_key(temp_state1, w, temp_state2, r * NUM_WORDS)
                   );
    AES_KEY_INDEPENDENT_ENC_ROUND_FINAL(temp_state2, temp_state1);
    add_round_key(temp_state1, w, temp_state2, num_rounds * NUM_WORDS);

    #pragma unroll
    for (size_t chunk=0; chunk<NUM_WORDS; chunk++) {
        PUT32((state_out + (chunk << 2)), temp_state2[chunk]);
    }
}

void decrypt(__private uchar state_in[BLOCK_SIZE],
             __local uint* w,
             __private uchar state_out[BLOCK_SIZE],
             unsigned int num_rounds) {

    uint temp_state1[NUM_WORDS], temp_state2[NUM_WORDS];

    #pragma unroll
    for (size_t chunk=0; chunk<NUM_WORDS; chunk++) {
        temp_state1[chunk] = GETU32((state_in + (chunk << 2)));
    }

    add_round_key(temp_state1, w, temp_state2, 0);
    INNER_AES_LOOP(
                   AES_KEY_INDEPENDENT_DEC_ROUND(temp_state2, temp_state1),
                   add_round_key(temp_state1, w, temp_state2, r * NUM_WORDS)
                   );
    AES_KEY_INDEPENDENT_DEC_ROUND_FINAL(temp_state2, temp_state1)
    add_round_key(temp_state1, w, temp_state2, num_rounds * NUM_WORDS);

    #pragma unroll
    for (size_t chunk=0; chunk<NUM_WORDS; chunk++) {
        PUT32((state_out + (chunk << 2)), temp_state2[chunk]);
    }
}


void copy_extkey_to_local(__local uint* local_w, __global uint* restrict w) {
    #pragma unroll
    for (size_t i = 0; i < MAX_EXKEY_SIZE_WORDS; ++i) {
        local_w[i] = w[i];
    }
}


__attribute__((reqd_work_group_size(1, 1, 1)))
__kernel void aesEncCipher(__global uchar* restrict in,
                           __global uint* restrict w,
                           __global uchar* restrict out,
                           unsigned int num_rounds,
                           unsigned int input_size) {
    __private uchar state_in[BLOCK_SIZE];
    __private uchar state_out[BLOCK_SIZE];
    __local uint __attribute__((numbanks(MAX_EXKEY_SIZE_WORDS), bankwidth(4))) local_w[MAX_EXKEY_SIZE_WORDS];
    copy_extkey_to_local(local_w, w);

    for (size_t blockid=0; blockid < input_size / BLOCK_SIZE; blockid++) {
       #pragma unroll
       for (size_t i = 0; i < BLOCK_SIZE; ++i) {
           size_t offset = blockid * BLOCK_SIZE + i;
           state_in[i] = in[offset];
       }
       encrypt(state_in, local_w, state_out, num_rounds);
       #pragma unroll
       for(size_t i = 0; i < BLOCK_SIZE; i++) {
           size_t offset = blockid * BLOCK_SIZE + i;
           out[offset] = state_out[i];
       }
    }
}

__attribute__((reqd_work_group_size(1, 1, 1)))
__kernel void aesDecCipher(__global uchar* restrict in,
                           __global uint* restrict w,
                           __global uchar* restrict out,
                           unsigned int num_rounds,
                           unsigned int input_size) {
    __private uchar state_in[BLOCK_SIZE];
    __private uchar state_out[BLOCK_SIZE];
    __local uint __attribute__((numbanks(MAX_EXKEY_SIZE_WORDS), bankwidth(4))) local_w[MAX_EXKEY_SIZE_WORDS];
    copy_extkey_to_local(local_w, w);
    finalize_inverted_key(local_w, num_rounds);

    for (size_t blockid=0; blockid < input_size / BLOCK_SIZE; blockid++) {
        #pragma unroll
        for (size_t i = 0; i < BLOCK_SIZE; ++i) {
            size_t offset = blockid * BLOCK_SIZE + i;
            state_in[i] = in[offset];
        }
        decrypt(state_in, local_w, state_out, num_rounds);
        #pragma unroll
        for(size_t i = 0; i < BLOCK_SIZE; i++) {
            size_t offset = blockid * BLOCK_SIZE + i;
            out[offset] = state_out[i];
        }
    }
}


void increment_counter(__private uchar* counter, size_t amount) {
    size_t n = BLOCK_SIZE, c = amount;
    #pragma unroll
    do {
        --n;
        c += counter[n];
        counter[n] = (uchar)(c & 0xFF);
        c >>= 8;
    } while (n);
}


__attribute__((reqd_work_group_size(1, 1, 1)))
__kernel void aesCipherCtr(__global uchar* restrict in,
                           __global uint* restrict w,
                           __global uchar* restrict out,
                           __global uchar* restrict IV,
                           unsigned int num_rounds,
                           unsigned int input_size) {
    __private uchar counter[BLOCK_SIZE];
    __private uchar outCipher[BLOCK_SIZE];
    __local uint __attribute__((numbanks(MAX_EXKEY_SIZE_WORDS), bankwidth(4))) local_w[MAX_EXKEY_SIZE_WORDS];
    copy_extkey_to_local(local_w, w);
    /* initialize counter */
    #pragma unroll
    for (size_t i = 0; i < BLOCK_SIZE; i++) {
        counter[i] = IV[i];
    }

    for (size_t blockid=0; blockid < input_size / BLOCK_SIZE; blockid++) {
        encrypt(counter, local_w, outCipher, num_rounds);
        #pragma unroll
        for (size_t i = 0; i < BLOCK_SIZE; i++) {
            size_t offset = blockid * BLOCK_SIZE + i;
            out[offset] = outCipher[i] ^ in[offset];
        }
        increment_counter(counter, 1);
    }
}
