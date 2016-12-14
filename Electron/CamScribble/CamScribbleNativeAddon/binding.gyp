{
  "targets": [
    {
      "target_name": "cam_scribble",
      "sources": [
        "src/init.cc",
        "src/VideoCaptureWrap.cc",
        "src/BigCanvas.cc",
        "src/CamScribbleWrap.cc",
        "src/PerspectiveCorrection.cc",
        "src/RetinaFilter.cc",
        "src/RunningAverage.cc"
        ],

      "libraries": [
        "-L/home/salar/OpenCV/opencv-3.1.0/output_lib/lib -lopencv_shape -lopencv_stitching -lopencv_objdetect -lopencv_superres -lopencv_videostab -lopencv_calib3d -lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_video -lopencv_photo -lopencv_ml -lopencv_imgproc -lopencv_flann -lopencv_viz -lopencv_core"
      ],
      # For windows

      "include_dirs": [
        "-I/usr/local/include/opencv -I/usr/local/include",
        "/usr/lib/node_modules/nan"
      ],

      "cflags!" : [ "-fno-exceptions"],
      "cflags_cc!": [ "-fno-rtti",  "-fno-exceptions"]
    }
  ]
}
