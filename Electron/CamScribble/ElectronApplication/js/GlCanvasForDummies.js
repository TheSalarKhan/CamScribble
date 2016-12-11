var vertexShaderSource = [""
    ,"attribute vec2 a_position;"
    ,"attribute vec2 a_texCoord;"
    ,"uniform vec2 u_resolution;"
    ,"varying vec2 v_texCoord;"
    ,"void main() {"
    // convert the rectangle from pixels to 0.0 to 1.0
    ,"vec2 zeroToOne = a_position / u_resolution;"
    // convert from 0->1 to 0->2
    ,"vec2 zeroToTwo = zeroToOne * 2.0;"
    // convert from 0->2 to -1->+1 (clipspace)
    ,"vec2 clipSpace = zeroToTwo - 1.0;"
    ,"gl_Position = vec4(clipSpace * vec2(1, -1), 0, 1);"
    // pass the texCoord to the fragment shader
    // The GPU will interpolate this value between points.
    ,"v_texCoord = a_texCoord;"
    ,"}"
].join("\n");

var fragmentShaderSource = [""
    ,"precision mediump float;"
    // our texture
    ,"uniform sampler2D u_image;"
    // the texCoords passed in from the vertex shader.
    ,"varying vec2 v_texCoord;"
    ,"void main() {"
    ,"gl_FragColor = texture2D(u_image, v_texCoord);"
    ,"}"
].join("\n");


// Loads a GLSL program for the context 'gl'.
function loadGLSLProgram(gl) {
  // setup GLSL program
  var program =
    Helpers.createProgramFromSources(gl, [vertexShaderSource, fragmentShaderSource]);

  // Tell it to use our program (pair of shaders)
  gl.useProgram(program);

  return program;
}

function initializeProgramInputs(canvas,gl,program) {
  // Create position buffer for vertex shader,
  // and upload the associated data.
  // this data corresponds to the 'a_position'
  // attribute.
  var positionBuffer = gl.createBuffer();
  var x1 = 0;
  var x2 = canvas.width;
  var y1 = 0;
  var y2 = canvas.height;
  var data = new Float32Array([
    x1, y1,
    x2, y1,
    x1, y2,
    x1, y2,
    x2, y1,
    x2, y2,
  ]);
  Helpers.assignAndBindBuffer(gl,positionBuffer,data,program,"a_position");


  // Create texture co-ordinate buffer, and upload data.
  // This data corresponds to 'a_texCoord' attribute.
  var texcoordBuffer = gl.createBuffer();
  data = new Float32Array([
      0.0,  0.0,
      1.0,  0.0,
      0.0,  1.0,
      0.0,  1.0,
      1.0,  0.0,
      1.0,  1.0,
  ]);
  Helpers.assignAndBindBuffer(gl,texcoordBuffer,data,program,"a_texCoord");


  // Create a texture and bind it to texture unit 0.
  // set some parameters on it and inform program about
  // the fact that the texture for 'u_image' is found in unit 0.
  var texture = gl.createTexture();
  gl.activeTexture(gl.TEXTURE0 + 0);
  gl.bindTexture(gl.TEXTURE_2D, texture);
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
  gl.uniform1i(gl.getUniformLocation(program, "u_image"), 0);




  // resize canvas.
  Helpers.resizeCanvasToDisplaySize(gl.canvas);

  // Tell WebGL how to convert from clip space to pixels,
  // and set the 'u_resolution' uniform.
  gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);
  gl.uniform2f(gl.getUniformLocation(program, "u_resolution"), gl.canvas.width, gl.canvas.height);
}

function initGLCanvas(canvas,width,height) {
  canvas.width = width;
  canvas.height = height;
  gl = canvas.getContext("webgl");
  if (!gl) {
    //webglLessonsHelper.showNeedWebGL(canvas);
    return;
  }

  // Create a GLSL program object.
  var program = loadGLSLProgram(gl);

  initializeProgramInputs(canvas,gl,program);

  // Clear the canvas
  gl.clearColor(0, 0, 0, 0);
  gl.clear(gl.COLOR_BUFFER_BIT);

  return gl;
}

Helpers = {
  assignAndBindBuffer: function(gl,buffer, data, program,variable) {
    // bind the buffer as ARRAY_BUFFER
    gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
    // Upload data to buffer
    gl.bufferData(gl.ARRAY_BUFFER,data,gl.DYNAMIC_DRAW);

    // look up where the vertex data needs to go.
    var variableLocation = gl.getAttribLocation(program, variable);
    // Turn on the position attribute
    gl.enableVertexAttribArray(variableLocation);
    // Tell the position attribute how to get data out of (ARRAY_BUFFER)
    var size = 2;          // 2 components per iteration
    var type = gl.FLOAT;   // the data is 32bit floats
    var normalize = false; // don't normalize the data
    var stride = 0;        // 0 = move forward size * sizeof(type) each iteration to get the next position, its used with interleaved data.
    var offset = 0;        // start at the beginning of the buffer
    gl.vertexAttribPointer(variableLocation, size, type, normalize, stride, offset);
  },

  renderUint8Array: function(gl,data,width,height) {
    gl.texImage2D(gl.TEXTURE_2D,0,gl.RGB,640,480,0,gl.RGB,gl.UNSIGNED_BYTE,data);
    gl.drawArrays(gl.TRIANGLES, 0, 6);
  },

  createProgramFromSources: function(gl, shaderSources, opt_attribs, opt_locations, opt_errorCallback) {
    var defaultShaderType = [
      "VERTEX_SHADER",
      "FRAGMENT_SHADER",
    ];
    var shaders = [];
    for (var ii = 0; ii < shaderSources.length; ++ii) {
      shaders.push(this.loadShader(gl, shaderSources[ii], gl[defaultShaderType[ii]], opt_errorCallback));
    }
    return this.createProgram(gl, shaders, opt_attribs, opt_locations, opt_errorCallback);
  },

  loadShader: function(gl, shaderSource, shaderType, opt_errorCallback) {
    var errFn = opt_errorCallback || this.error;
    // Create the shader object
    var shader = gl.createShader(shaderType);

    // Load the shader source
    gl.shaderSource(shader, shaderSource);

    // Compile the shader
    gl.compileShader(shader);

    // Check the compile status
    var compiled = gl.getShaderParameter(shader, gl.COMPILE_STATUS);
    if (!compiled) {
      // Something went wrong during compilation; get the error
      var lastError = gl.getShaderInfoLog(shader);
      errFn("*** Error compiling shader '" + shader + "':" + lastError);
      gl.deleteShader(shader);
      return null;
    }

    return shader;
  },

  createProgram: function(gl, shaders, opt_attribs, opt_locations, opt_errorCallback) {
    var errFn = opt_errorCallback || this.error;
    var program = gl.createProgram();
    shaders.forEach(function(shader) {
      gl.attachShader(program, shader);
    });
    if (opt_attribs) {
      opt_attribs.forEach(function(attrib, ndx) {
        gl.bindAttribLocation(
            program,
            opt_locations ? opt_locations[ndx] : ndx,
            attrib);
      });
    }
    gl.linkProgram(program);

    // Check the link status
    var linked = gl.getProgramParameter(program, gl.LINK_STATUS);
    if (!linked) {
        // something went wrong with the link
        var lastError = gl.getProgramInfoLog(program);
        errFn("Error in program linking:" + lastError);

        gl.deleteProgram(program);
        return null;
    }
    return program;
  },

  error: function(msg) {
    if (topWindow.console) {
      if (topWindow.console.error) {
        topWindow.console.error(msg);
      } else if (topWindow.console.log) {
        topWindow.console.log(msg);
      }
    }
  },

  resizeCanvasToDisplaySize: function(canvas, multiplier) {
    multiplier = multiplier || 1;
    var width  = canvas.clientWidth  * multiplier | 0;
    var height = canvas.clientHeight * multiplier | 0;
    if (canvas.width !== width ||  canvas.height !== height) {
      canvas.width  = width;
      canvas.height = height;
      return true;
    }
    return false;
  },
}



function GlCanvasForDummies(canvasObject,width,height) {
  this.gl = initGLCanvas(canvasObject,width,height);

}

/**
  This function will render an RGB24 format buffer 'data', to the canvas.
*/
GlCanvasForDummies.prototype.renderImage = function(data,width,height) {
  Helpers.renderUint8Array(this.gl,data,width,height);
}


module.exports = GlCanvasForDummies;
