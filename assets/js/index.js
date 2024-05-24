// 
"use strict";
import { ApiService, createBreadCrumb, createCard} from "./scripts.js";
// pasar info desde api /api/index respuesta se pasa en index
let index = {};

// funcion asyncrona porque se van a estar haciendo peticiones a la api, hasta que no devuela la infromacion no se reenvia a donde se solicita
export async function iniciarIndex() {
    // cambiar titulo de html en cada pagina
    document.title = 'Home ESP32DT v1';
    //llamar funcion para crear breadcrum
    createBreadCrumb('Dashboard', 'Information', 'Dashboard');

    /* // Demo
    const url =`http://${host}/api/index`;
    // para hacer 
    await fetch(
        url, {
        method: 'GET',
        headers: {
            'Accept': 'application/json'
        }
    }
    )
        .then((res) => res.json())// respuesta se pasa a jsopn
        .then((datos) => {
            index = datos
        })
        .catch((error) => console.log(error))
    console.log(index); */
const getIndex= new ApiService('index') // instancia una clase
const resp=await getIndex.getApiData();
console.log(resp);
index=resp;
console.log(index);
//llenar html desde la api
// se crea la tarjeta para la serie de la esp32 
createCard('#serialCard','customers-card','Serial','cpu-fill','serial',index.info.serial,'Device serial number')
createCard('#deviceCard','sales-card','Device','tablet','device',index.device,'Device Type')
createCard('#HardwareVersion','sales-card','Versión Hardware','cpu-fill','HardwareVersion',index.info.Versión_del_Hardware,'Version beta')
createCard('#Fabricante','sales-card','Fabricante','cpu-fill','Fabricante',index.info.Fabricante,'Básculas y Balanzas LTDA')

}//,'/assets/img/card.jpg'