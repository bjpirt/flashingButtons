var SerialPort   = require('serialport').SerialPort;
var EventEmitter = require("events").EventEmitter;
var util         = require("util");

var FlashingButtons = function(config){
  this.port = config.port;
  this.baud = config.baud;
  this.connect();
  this.buffer = '';
  EventEmitter.call(this);
}

util.inherits(FlashingButtons, EventEmitter);

var p = FlashingButtons.prototype;

p.connect = function(){
  var self = this;
  this.conn = new SerialPort(this.port, {baudrate: this.baud});
  this.conn.on('data', function(d){ self.handleData(d); });
  this.conn.on("error", function (err) { self.emit('error', err); });
}

p.processMessage = function(msg){
  var startup = msg.match(/Starting flashingButtons \(([0-9]+) buttons\)/)
  if(startup){
    this.emit('connected');
    this.buttonCount = parseInt(startup[1]);
  }
  var press = msg.match(/PRESS ([0-9]+)/)
  if(press){
    this.emit('press', {button: parseInt(press[1])});
  }
  var release = msg.match(/RELEASE ([0-9]+)/)
  if(release){
    this.emit('release', {button: parseInt(release[1])});
  }
}

p.handleData = function(data){
  if(data) this.buffer += data.toString();
  var split = this.buffer.split("\r\n");
  if(split.length > 1){
    this.buffer = this.buffer.replace(/.*\r\n/, '');
    this.processMessage(split[0]);
    this.handleData();
  }
}

exports.FlashingButtons = FlashingButtons;