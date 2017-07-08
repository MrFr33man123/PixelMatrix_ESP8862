const char index_html[] PROGMEM = R"(<head>
<title>PixelMatrix Control Site</title>
<meta name='viewport' content='width=device-width, initial-scale=1'>
<style>
.color-sel { width: 24px; height: 24px; text-align: center;}
.pixel-sel { width: 48px; height: 48px; background-color: black; }
button { font-size: 16px; }
select { font-size: 16px; }
p { font-size:18px; font-style:normal; line-height:0px; }
table { border-color:lightgray; border-collapse:collapse; }
</style>
</head>
<body>
<script>
var nh=32,i,j,cc='rgb(255, 255, 255)';
var cc_elem;
function w(s) { document.write(s); }
function id(s){ return document.getElementById(s); }
function mark(e) {e.innerHTML='&#10004;'; }
function cs_click(e) {
  var bg=e.target.style.background;
  cc_elem.innerHTML='';
  mark(e.target);
  cc=bg;
  cc_elem=e.target;
}
w('<table border=1>');
idx=0;
j=0;
for(i=0;i<3;i++) {
w('<tr>');
for(j=0;j<(nh/2);j++,idx++) {
w('<td class=color-sel id=cs'+idx+'></td>');
var obj=id('cs'+idx);
if(i==0) { var hue=(j/nh)*360;
obj.style.background='hsl('+hue+',100%,50%)'; }
else if(i==1){ var hue=(nh-j)/nh*360;
obj.style.background='hsl('+hue+',100%,50%)'; }
else { var lit=(j/((nh/2)-1))*100; 
obj.style.background='hsl(0,0%,'+lit+'%)'; }
obj.addEventListener('click',cs_click);
}
w('</tr>');

}
w('</table>');
cc_elem=id('cs'+((nh+(nh/2))-1));
mark(cc_elem);
</script>
<hr />
<p>Funktions Auswahl / Animation<select id=ani_number onchange="animationchange();"><option value="next">Next</option><option value=0>Text n Draw</option><option value=6>Demo Mode On</option><option value=7>Demo Mode Off</option><option value=1>Test</option><option value=2>Rainbow</option><option value=3>Wipe</option><option value=4>Cross</option><option value=5>Pictures</option><option value=8>Tunnel</option></select><br> Set pixel color: <button id='btn_fill'>Fill</button> <button id='btn_clear'>Clear</button> <button id='btn_submit'><b>Submit</b></button></p>
<script>
function ps_click(e) { if(e.target.style.background==cc) e.target.style.background='rgb(0,0,0)'; else e.target.style.background=cc; }
w('<table border=1 style="border-collapse: separate; border-spacing: 2px;">');
idx=0;
for(i=0;i<8;i++) {
w('<tr>');
for(j=0;j<8;j++,idx++) {
w('<td class=pixel-sel id=ps'+idx+'></td>');
id('ps'+idx).addEventListener('click',ps_click);

id('ps'+idx).addEventListener('click', function(e) {
var comm='/cc?pixels=';
for(i=0;i<64;i++) comm+=rgb2cc(id('ps'+i).style.background);
show_comm(comm);
send_comm(comm);
});
}
w('</tr>');
}
w('</table>');
</script>
<p>Pre-defined patterns:</p>
<button id='btn_WIN'>Win</button> <button id='btn_pumpkin'>Pumpkin</button> <button id='btn_skull'>Creeper</button> <button id='btn_heart'>Heart</button> <button id='btn_checker'>Checker</button><br>
<br><button id='btn_scroll'>Text Anzeigen</button><input id="scroll_txt" type="text" name="scrolltext"><br>
Helligkeit<input id="brightness" type="range" min="5" max="255" step="5" value="80" onchange="brightnesschange();" oninput="updateTextInputBright(this.value);" style="width: 250px"><input type="text" id="brightness_txt" value="80" size="3" style="width: 40px;"><br>
Speed<input id="speed" type="range" min="5" max="500" step="5" value="60" onchange="speedchange(this.value);" oninput="updateTextInputSpeed(this.value);") style="width: 250px"><input type="text" id="speed_txt" value="40" size="4" style="width: 40px;"><br>
</p><input type=checkbox id='cb_comm'> Show HTTP Get command
</p>
<label id='lbl_comm'></label>
<script>
function rgb2cc(rgb) {
if(!rgb) return '000000';
rgb = rgb.match(/^rgba?\((\d+),\s*(\d+),\s*(\d+)(?:,\s*(\d+))?\)$/);
function hex(x) { return ('0' + parseInt(x).toString(16)).slice(-2); }
return '' + hex(rgb[1]) + hex(rgb[2]) + hex(rgb[3]);
}
id('btn_fill').addEventListener('click',function(e) {
for(i=0;i<64;i++) id('ps'+i).style.background = cc;
});
id('btn_clear').addEventListener('click',function(e) {
for(i=0;i<64;i++) id('ps'+i).style.background = 'rgb(0, 0, 0)';
});

function send_comm(comm) {
var xhr=new XMLHttpRequest();
xhr.onreadystatechange=function() {
if(xhr.readyState==4 && xhr.status==200) {
var jd=JSON.parse(xhr.responseText);
// jd stores the response data
}
};
comm = 'http://'+window.location.hostname+comm;
xhr.open('GET',comm,true);
xhr.send();
}
function show_comm(comm) {
comm = 'http://'+window.location.hostname+comm;
if(id('cb_comm').checked) id('lbl_comm').innerHTML=comm;
else id('lbl_comm').innerHTML='';
}
id('btn_submit').addEventListener('click', function(e) {
var comm='/cc?pixels=';
for(i=0;i<64;i++) comm+=rgb2cc(id('ps'+i).style.background);
show_comm(comm);
send_comm(comm);
});
id('btn_scroll').addEventListener('click', function(e) {
var comm='/cc?scrolltext='+document.getElementById("scroll_txt").value;
show_comm(comm);
send_comm(comm);
});


function updateTextInputBright(val) {
          document.getElementById('brightness_txt').value=val; 
}
function updateTextInputSpeed(val) {
          document.getElementById('speed_txt').value=val; 
}
function set_icon(icon, fg, bg) {
for(i=0;i<64;i++) id('ps'+i).style.background=(icon[i]>0)?fg:bg;
}
function set_icon2(icon) {
for(i=0;i<64;i++) id('ps'+i).style.background=icon[i];
}
id('btn_WIN').addEventListener('click', function(e) {
var WIN=[
'#FF0000','#FF0000','#FF0000','#FF0000','#00FF00','#00FF00','#00FF00','#00FF00',
'#FF0000','#FF0000','#FF0000','#FF0000','#00FF00','#00FF00','#00FF00','#00FF00',
'#FF0000','#FF0000','#FF0000','#FF0000','#00FF00','#00FF00','#00FF00','#00FF00',
'#FF0000','#FF0000','#FF0000','#FF0000','#00FF00','#00FF00','#00FF00','#00FF00',
'#0000FF','#0000FF','#0000FF','#0000FF','#FFFF00','#FFFF00','#FFFF00','#FFFF00',
'#0000FF','#0000FF','#0000FF','#0000FF','#FFFF00','#FFFF00','#FFFF00','#FFFF00',
'#0000FF','#0000FF','#0000FF','#0000FF','#FFFF00','#FFFF00','#FFFF00','#FFFF00',
'#0000FF','#0000FF','#0000FF','#0000FF','#FFFF00','#FFFF00','#FFFF00','#FFFF00'
];
set_icon2(WIN);
});
id('btn_heart').addEventListener('click', function(e) {
var heart=[
0,0,0,0,0,0,0,0,
0,1,1,0,1,1,0,0,
1,1,1,1,1,1,1,0,
1,1,1,1,1,1,1,0,
1,1,1,1,1,1,1,0,
0,1,1,1,1,1,0,0,
0,0,1,1,1,0,0,0,
0,0,0,1,0,0,0,0
];
set_icon(heart, '#FF0000', '#000020');
});
id('btn_checker').addEventListener('click', function(e) {
var heart=[1,0,1,0,1,0,1,0,
0,1,0,1,0,1,0,1,
1,0,1,0,1,0,1,0,
0,1,0,1,0,1,0,1,
1,0,1,0,1,0,1,0,
0,1,0,1,0,1,0,1,
1,0,1,0,1,0,1,0,
0,1,0,1,0,1,0,1];
set_icon(heart, '#FF0000', '#000020');
});
id('btn_skull').addEventListener('click', function(e) {
var skull=[
'#00AA00','#00AA00','#00AA00','#00AA00','#00AA00','#00AA00','#00AA00','#00AA00',
'#00AA00','#00AA00','#00AA00','#00AA00','#00AA00','#00AA00','#00AA00','#00AA00',
'#00AA00','#000000','#000000','#00AA00','#00AA00','#000000','#000000','#00AA00',
'#00AA00','#000000','#000000','#00AA00','#00AA00','#000000','#000000','#00AA00',
'#00AA00','#00AA00','#00AA00','#000000','#000000','#00AA00','#00AA00','#00AA00',
'#00AA00','#00AA00','#000000','#000000','#000000','#000000','#00AA00','#00AA00',
'#00AA00','#00AA00','#000000','#000000','#000000','#000000','#00AA00','#00AA00',
'#00AA00','#00AA00','#000000','#00AA00','#00AA00','#000000','#00AA00','#00AA00'];
set_icon2(skull);
});
id('btn_pumpkin').addEventListener('click', function(e) {
var pumpkin=['#000000','#008000','#000000','#000000','#000000','#000000','#000000','#000000',
'#000000','#000000','#008000','#000000','#000000','#000000','#000000','#000000',
'#000000','#802000','#802000','#802000','#000000','#000000','#000000','#000000',
'#802000','#000000','#000000','#000000','#802000','#000000','#000000','#000000',
'#802000','#000000','#000000','#000000','#802000','#000000','#000000','#000000',
'#802000','#000000','#000000','#000000','#802000','#000000','#000000','#000000',
'#000000','#802000','#802000','#802000','#000000','#000000','#000000','#000000',
'#000000','#000000','#000000','#000000','#000000','#000000','#000000','#000000'];
set_icon2(pumpkin);
});

function animationchange(ani_nu) {
hexcolor = rgb2cc(cc);
var ani_nu = document.getElementById("ani_number").value;
var comm = '/ani';
if(ani_nu != "next"){comm += '?ani'+'='+ani_nu;
    comm += '&color='+hexcolor;
  }
show_comm(comm +" Animation:"+ ani_nu+" hexcolor:"+ hexcolor);
send_comm(comm);
}

function brightnesschange(brightness) {
var comm='/cc?brightness='+document.getElementById("brightness").value;
show_comm(comm);
send_comm(comm);
}

function speedchange(speed) {
var comm='/cc?speed='+document.getElementById("speed").value;
show_comm(comm + "Wert von CC: " + (hexcolor = rgb2cc(cc)));
send_comm(comm);
}
</script>
</body>)";

