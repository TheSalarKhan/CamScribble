cmd_Release/cam_scribble.node := ln -f "Release/obj.target/cam_scribble.node" "Release/cam_scribble.node" 2>/dev/null || (rm -rf "Release/cam_scribble.node" && cp -af "Release/obj.target/cam_scribble.node" "Release/cam_scribble.node")
