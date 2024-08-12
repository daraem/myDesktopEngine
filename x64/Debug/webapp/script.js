let canvas = document.getElementById("canvas")
canvas.width = window.innerWidth;
canvas.height = window.innerHeight;

let canvaWidth = canvas.width;
let canvaHeight = canvas.height; 
let monitorWidth = screen.width;
let monitorHeight = screen.height;

let proportionX = canvaWidth / monitorWidth;
let proportionY = canvaHeight / monitorHeight;

const ctx = canvas.getContext("2d");

let rawData = null;
let numbers = [0, 0, 0];
const img = new Image();

window.electronAPI.receiveData((event, data) => {
  numbers = [];
  if(rawData != data.message) {
    for(let i = 0; i<data.message.split(':').length; i++) {
        numbers.push(data.message.split(':')[i].replace(/\D/g, ""))}
    rawData = data.message;
    alert(rawData)
  }
});