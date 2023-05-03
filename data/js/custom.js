var targetUrl = `ws://${window.location.hostname}/ws`;
var websocket;
window.addEventListener('load', onLoad);

function onLoad() {
  initializeSocket();
}

function initializeSocket() {
  console.log('Opening WebSocket connection to ESP32...');
  websocket = new WebSocket(targetUrl);
  websocket.onopen = onOpen;
  websocket.onclose = onClose;
  websocket.onmessage = onMessage;
}
function onOpen(event) {
  console.log('Starting connection to server..');
}
function onClose(event) {
  console.log('Closing connection to server..');
  setTimeout(initializeSocket, 2000);
}
function onMessage(event) {
  console.log("WebSocket message received:", event)
}

function sendMessage(message) {
  websocket.send(message);
}

let pressed = {
  w:false,
  a:false,
  s:false,
  d:false
}

window.addEventListener('keydown', (press)=>{
  if ((press.key == 'w' || press.key == 'ArrowUp') && !pressed.w) {
    sendMessage('up');
    pressed.w = true;
  }
  else if ((press.key == 'a' || press.key == 'ArrowLeft') && !pressed.a) {
    sendMessage('left');
    pressed.a = true;
  }
  else if ((press.key == 'd' || press.key == 'ArrowRight') && !pressed.d) {
    sendMessage('right');
    pressed.d = true;
  }
  else if ((press.key == 's' || press.key == 'ArrowDown') && !pressed.s) {
    sendMessage('down');
    pressed.s = true;
  }
})

window.addEventListener('keyup', (press)=>{
  if (press.key == 'w' || press.key == 'ArrowUp') {
    sendMessage('stop');
    pressed.w = false;
  }
  else if (press.key == 'a' || press.key == 'ArrowLeft') {
    sendMessage('stop');
    pressed.a = false;
  }
  else if (press.key == 'd' || press.key == 'ArrowRight') {
    sendMessage('stop');
    pressed.d = false;
  }
  else if (press.key == 's' || press.key == 'ArrowDown') {
    sendMessage('stop');
    pressed.s = false;
  }
})

/*
O-Pad/ D-Pad Controller and Javascript Code
*/
// Prevent scrolling on every click!
// super sweet vanilla JS delegated event handling!
window.addEventListener('touchstart', touchStartHandler);
window.addEventListener('touchend', touchEndHandler);
window.addEventListener('touchmove', touchMoveHandler);

function touchStartHandler(event) {
  var direction = event.target.dataset.direction;
  console.log('Touch Start :: ' + direction)
  sendMessage(direction);
}

function touchEndHandler(event) {
  const stop_command = 'stop';
  var direction = event.target.dataset.direction;
  console.log('Touch End :: ' + direction)
  sendMessage(stop_command);
}

function touchMoveHandler(event) {
  // Set call preventDefault()
  event.preventDefault();
}

$(document).ready(function(){
  window.disableSelection();
});

$.fn.extend({
  disableSelection: function() {
      this.each(function() {
          this.onselectstart = function() {
              return false;
          };
          this.unselectable = "on";
          $(this).css('-moz-user-select', 'none');
          $(this).css('-webkit-user-select', 'none');
      });
      return this;
  }
});

// document.querySelectorAll('.control').forEach(item => {
//   item.addEventListener('touchstart', touchStartHandler);
  
// })

// document.querySelectorAll('.control').forEach(item => {
//   item.addEventListener('touchend', touchEndHandler)
// })



