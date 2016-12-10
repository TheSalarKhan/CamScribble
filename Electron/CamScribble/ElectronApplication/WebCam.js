function WebCam(index) {
	this.camScribble = require('cam_scribble');
	this.camera = null;
	this.camIndex = index;
}


WebCam.prototype.open = function() {
	this.camera = new this.camScribble.VideoCapture(this.camIndex);
}


WebCam.prototype.read = function() {
	return this.camera.read();
}

WebCam.prototype.readIntoBuffer = function(buffer) {
	return this.camera.readIntoBuffer(buffer);
}


WebCam.prototype.close = function() {
	this.camera.release();

	this.camera = null;
}



module.exports = WebCam;