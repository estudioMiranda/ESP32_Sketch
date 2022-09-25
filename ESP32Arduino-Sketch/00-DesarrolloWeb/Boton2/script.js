var estado = false;

function onOff() {
  estado = !estado;
  var salida = document.getElementById("valor");
  salida.innerHTML = estado;
  console.log("botón pulsado", estado)
};