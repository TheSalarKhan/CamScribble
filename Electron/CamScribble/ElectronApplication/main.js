const {app, BrowserWindow} = require('electron')

let win

function main() {
    win = new BrowserWindow({
        width:640,
        height:480
    });

    win.loadURL(`file://${__dirname}/main.html`);

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






