{
  "targets": [
    {
      "target_name": "farsight",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "sources": [
        "./src/unit/GameObject.cc",
        "./src/Farsight.cc",
        "./src/Offsets.cc",
        "./src/Utils.cc",
        "./src/index.cc"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS', 'WIN32_LEAN_AND_MEAN' ],
    }
  ]
}