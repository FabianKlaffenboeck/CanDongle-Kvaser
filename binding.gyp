{
  'variables': {
    'openssl_fips': ''
  },
  'targets': [{
    'target_name': 'candongle_kvaser',
    'include_dirs': ["<!(node -p \"require('node-addon-api').include_dir\")"],
    'cflags!': [ '-fno-exceptions' ],
    'cflags_cc!': [ '-fno-exceptions' ],
    "defines": ["NAPI_CPP_EXCEPTIONS"],
    'conditions': [
      ['OS=="win"',
        {
          'defines': ['CHECK_NODE_MODULE_VERSION'],
          'sources': [
            'src/candongle_kvaser_win.cpp'
          ],
          "libraries": [
            "src/Canlib/Lib/MS/canlib32.lib",
            "src/Canlib/Lib/x64/canlib32.lib"
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