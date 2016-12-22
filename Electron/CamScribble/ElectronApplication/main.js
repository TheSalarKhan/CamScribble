const {app, BrowserWindow,ipcMain} = require('electron');
const CamScribble = require('cam_scribble').CamScribble;

let win

function main() {
  win = new BrowserWindow({
      width:800,
      height:800,
      titleBarStyle: 'hidden',
      resizable: true
  });

  win.loadURL(`file://${__dirname}/calibrateCamera.html`);

  win.setMenu(null);

  win.webContents.openDevTools();

  win.on('closed',() => {
      win = null;
  });
}


app.on('ready',main);


app.on('window-all-closed',() => {
    if (process.platform !== 'darwin') {
        app.quit()
    }
});


app.on('activate', () => {

    if(win==null) {
        main();
    }
});



ipcMain.on('a-message', (event, arg) => {
  console.log(arg)  // prints "ping"
  //event.returnValue = 'pong'
})
