{
  "targets": [
    {
      "target_name": "kvaser_can",
      "sources": [
        "src/kvaser_can.cpp"
      ],
      "conditions": [
        [ 'OS=="win"', {
          "libraries": [
            # Windows-specific path to the library
            "<(module_root_dir)/Canlib/Lib/x64/canlib32.lib",
            "<(module_root_dir)/Canlib/Lib/MS/canlib32.lib"
          ]
        }]
      ],
      "include_dirs": [
        "<(module_root_dir)/Canlib/INC"
      ],
      "defines": [
        # Any preprocessor definitions you need
      ]
    }
  ]
}
