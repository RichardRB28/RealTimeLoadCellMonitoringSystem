//------------------obtener valores de los id del html-----------------//

const calib = document.querySelector('#calibracion');
const light_calib = document.querySelector('#calib_state_led');
const light_confir = document.querySelector('#confirm_state_led');
const confirmar = document.querySelector('#confirmar');


//-------------------------- Agregar un event listener para detectar clics en el botón-------------------//
//calib.addEventListener('click', calib);
//light_calib.style.backgroundColor = 'red';
//calib.addEventListener('click', function() { light_calib.style.backgroundColor = '#00ff40'; });

confirmar.addEventListener('click', led_state_confirm);
//-----------------------FUNCION PARA ENVIAR RESPUESTA EN FORMATO JSON----------------------------

/* espera a que este la solicitud completa para enviar la peticion al servidor*/

async function send_to_server_request(url) {
    try {
        let ans = await fetch(url);
        return await ans.json();

    } catch (error) {
        console.log(error);
    }
}
//-----esperará a que se presione el pulsador, cuando esto pase enviará una petición al servidor esp32---//

async function led_state_confirm(e) {
    let source = e.target.name;
    let url = '/cambiar_color/' + source + '?status:';
    if (!e.target.active) {
        url += 'true';
        light_calib.active();
    } else {
        url += 'false';
    }
    console.log("Sending to " + url);
    let response = await send_to_server_request(url);
    console.log(response);
}
/*
// se pasa la funcion 
async function led_state_confirm(e) {
    let Source_event = e.target.name;
    let url = '/button/' + Source_event + '?status=';
    if (e.target) {
        url += 'true';
    } else {
        url += 'false';
    }
    console.log("transfiriendo a: "+ url);
    let response = await send_to_server_request(url);
    console.log(response);
}
//   function*/