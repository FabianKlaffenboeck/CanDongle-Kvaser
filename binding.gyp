{
  'variables': {
    'openssl_fips': ''
  },
  'targets': [{
    'target_name': 'kvaser_can',
    'include_dirs': ["<!(node -p \"require('node-addon-api').include_dir\")"],
    'cflags!': [ '-fno-exceptions' ],
    'cflags_cc!': [ '-fno-exceptions' ],
    "defines": ["NAPI_CPP_EXCEPTIONS"],
    'conditions': [
      ['OS=="win"',
        {
          'defines': ['CHECK_NODE_MODULE_VERSION'],
          "include_dirs": [
            "Canlib/INC"
          ],
          "libraries": [
              "<(module_root_dir)/Canlib/Lib/x64/canlib32.lib",
              "<(module_root_dir)/Canlib/Lib/MS/canlib32.lib"
          ],
          'sources': [
            'src/candongle_kvaser_win.cpp'
          ],
          'msvs_settings': {
            'VCCLCompilerTool': {
              'ExceptionHandling': '1',
              'DisableSpecificWarnings': [ '4530', '4506' ],
            }
          }
        }
      ]
    ]
  }],
}
