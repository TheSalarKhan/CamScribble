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
        "src/RunningAverage.cc",
        "src/Matrix.cc"
        ],
	
	"include_dirs": [
		"/home/salar/OpenCV/opencv-3.1.0/output_lib/include",
		"/usr/local/include",
		"/usr/local/lib/node_modules/nan"
      	],
	
	"link_settings": {
		"libraries": [
			"-lopencv_core",
			"-lopencv_highgui",
			"-lopencv_videoio",
			"-lopencv_imgcodecs",
			"-lopencv_video",
		],
		"ldflags": ["-L/home/salar/OpenCV/opencv-3.1.0/output_lib/lib"]
	},
	
#"libraries": [
#        "-lopencv_shape -lopencv_stitching -lopencv_objdetect -lopencv_superres -lopencv_videostab -lopencv_calib3d -lopencv_features2d  -#lopencv_photo -lopencv_ml -lopencv_imgproc -lopencv_flann -lopencv_viz -lopencv_core"
#      ],

      

      "cflags!" : [ "-fno-exceptions"],
      "cflags_cc!": [ "-fno-rtti",  "-fno-exceptions"]
    }
  ]
}
