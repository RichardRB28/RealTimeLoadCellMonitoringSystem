"use strict"; // para uso estricto de var, const, let para variables
const urlActual = window.location;
export const url = /^(\w+):\/\/([^\/]+)([^]+)$/.exec(urlActual);
export const host = url[2] === '127.0.0.1:5500' ? '192.168.10.62' : url[2]; // ip - mdns

//---------- funcion para construir html desde java script--------//
export function builder(Node) {
    // nodo indefinido
    if (typeof Node === 'undefined') {
        return document.createTextNode("");
    }
    // si node es un string
    if (typeof Node === 'string') {
        return document.createTextNode(Node);
    }
    // Si es un html comparamos por tagname y retorno el mismo html
    if (Node.tagName) {
        return Node;
    }
    // si enviamos un objeto en JS se construye el html segun el tipo que esté llegando
    const element = document.createElement(Node.type);// se crea el elemento segun el type que se pasa
    // si llega props ( propiedades)
    if (Node.props) {
        for (const prop in Node.props) {
            if (typeof Node.props[prop] === 'function' || typeof Node.props[prop] === 'object') {
                element[prop] = Node.props[prop];
            }
            else if (prop.includes('bs')) {// parametros personalizados bsData
                element.dataset[prop] = Node.props[prop];// dataset se define una propiedad personalizada
            }
            else {
                element.setAttribute(prop, Node.props[prop]);
            }
        }
    }
    // si llegan hijos
    if (Node.children) {// se crea una copia del array original para que no se sobreescriban
        // Node.children.map(builder).forEach(Child => element.appendChild(Child)); 
        Node.children.forEach(child => {
            const childElement = builder(child);
            element.appendChild(childElement);
        });
    }
    if (Node.events) {
        for (const event in Node.events) {
            element.addEventListener(event, Node.events[event](), false);// captura eventos
        }
    }
    return element;
}
// Funcion para crear header desde js
export function createHeader() {
    const contenedor = document.querySelector('#header');// captura por id 'header'
    const div = builder({
        type: 'div',
        props: { class: 'd-flex align-items-center justify-content-between' },
        children: [
            {
                type: 'a',// link
                props: { class: 'logo d-flex align-items-center', href: '/' },
                children: [{
                    type: 'img',
                    props: { src: 'assets/img/logo.png', alt: 'logo' }
                },
                {
                    type: 'span',
                    props: { class: 'd-none d-lg-block fs-5' },
                    children: [
                        {
                            type: 'a',
                            children: ['DigitalTwins v1.0']
                        }
                    ]
                }]
            },
            {
                type: 'i',
                props: { class: 'bi-list toggle-sidebar-btn' }
            }
        ]
    });
    contenedor.appendChild(div);

    // Barra de navegacion
    const nav = builder({
        type: 'nav',
        props: { class: 'header-nav ms-auto' },
        children: [
            {
                type: 'ul',
                props: { class: 'd-flex align-items-center' },
                children: [
                    {
                        type: 'li',
                        props: { class: 'nav-item dropdown' },
                        children: [
                            {
                                type: 'a',
                                props: {
                                    class: 'nav-link nav-icon',
                                    href: '#',
                                    bsToggle: 'dropdown'
                                },
                                children: [
                                    {
                                        type: 'i',
                                        props: {
                                            class: 'bi',
                                            bsToggle: 'tooltip',
                                            bsPlacement: 'bottom',
                                            bsOriginalTitle: 'Wifi Connection',
                                            id: 'wifiStatus'
                                        }
                                    }
                                ]
                            }
                        ]
                    },
                    {
                        type: 'li',
                        props: { class: 'nav-item dropdown' },
                        children: [
                            {
                                type: 'a',
                                props: {
                                    class: 'nav-link nav-icon',
                                    href: '#',
                                    bsToggle: 'dropdown'
                                },
                                children: [
                                    {
                                        type: 'i',
                                        props: {
                                            class: 'bi',
                                            bsToggle: 'tooltip',
                                            bsPlacement: 'bottom',
                                            bsOriginalTitle: 'WiFi RSSI',
                                            id: 'rssiStatus'
                                        }
                                    }
                                ]
                            }
                        ]
                    },
                    {
                        type: 'li',
                        props: { class: 'nav-item dropdown' },
                        children: [
                            {
                                type: 'a',
                                props: {
                                    class: 'nav-link nav-icon',
                                    href: '#',
                                    bsToggle: 'dropdown'
                                },
                                children: [
                                    {
                                        type: 'i',
                                        props: {
                                            class: 'bi',
                                            bsToggle: 'tooltip',
                                            bsPlacement: 'bottom',
                                            bsOriginalTitle: 'Conexión MQTT',
                                            id: 'mqttStatus'
                                        }
                                    }
                                ]
                            }
                        ]
                    },
                    {
                        type: 'li',
                        props: { class: 'nav-item dropdown pe-3' },
                        children: [
                            {
                                type: 'a',
                                props: {
                                    class: 'nav-link nav-profile d-flex align-items-center pe-0',
                                    href: '#',
                                    bsToggle: 'dropdown'
                                },
                                children: [
                                    {
                                        type: 'span',
                                        props: { class: 'fs-5 d-none d-md-block dropdown-toggle ps-2' },
                                        children: [{
                                            type: 'a',
                                            children: ['']
                                        },
                                        {
                                            type: 'i', props: { class: 'bi-chevron-double-down' }
                                        }

                                        ]
                                    }
                                ]
                            },
                            {
                                type: 'ul',
                                props: { class: 'dropdown-menu dropdown-menu-end dropdown-menu-arrow profile' },
                                children: [
                                    {
                                        type: 'li',
                                        props: { class: 'dropdown-header' },
                                        children: [
                                            {
                                                type: 'h6',
                                                children: ['¡Welcome!']
                                            },
                                            {
                                                type: 'span',
                                                children: ['Options']
                                            }

                                        ]
                                    },
                                    {
                                        type: 'li',
                                        children: [
                                            {
                                                type: 'hr',
                                                props: { class: 'dropdown-divider' }
                                            }
                                        ]
                                    },
                                    {
                                        type: 'li',
                                        children: [
                                            {
                                                type: 'a',
                                                props: { class: 'dropdown-item d-flex align-items-center', href: 'esp-admin' },
                                                children: [
                                                    { type: 'i', props: { class: 'bi-person-fill' } },
                                                    { type: 'span', children: ['Profile'] }
                                                ]
                                            }
                                        ]
                                    },
                                    {
                                        type: 'li',
                                        children: [
                                            {
                                                type: 'hr',
                                                props: { class: 'dropdown-divider' }
                                            }
                                        ]
                                    },
                                    {
                                        type: 'li',
                                        children: [
                                            {
                                                type: 'a',
                                                props: { class: 'dropdown-item d-flex align-items-center', href: 'esp-device' },
                                                children: [
                                                    { type: 'i', props: { class: 'bi-gear-fill' } },
                                                    { type: 'span', children: ['Settings'] }
                                                ]
                                            }
                                        ]
                                    },
                                    {
                                        type: 'li',
                                        children: [
                                            {
                                                type: 'hr',
                                                props: { class: 'dropdown-divider' }
                                            }
                                        ]
                                    },
                                    {
                                        type: 'li',
                                        children: [
                                            {
                                                type: 'a',
                                                props: {
                                                    class: 'dropdown-item d-flex align-items-center',
                                                    href: 'https://www.bascosta.com/contacto/',
                                                    target: '_blank'
                                                },
                                                children: [
                                                    { type: 'i', props: { class: 'bi-question-circle-fill' } },
                                                    { type: 'span', children: ['¿Help?'] }
                                                ]
                                            }
                                        ]
                                    },
                                    {
                                        type: 'li',
                                        children: [
                                            {
                                                type: 'hr',
                                                props: { class: 'dropdown-divider' }
                                            }
                                        ]
                                    },
                                    {
                                        type: 'li',
                                        children: [
                                            {
                                                type: 'a',
                                                props: {
                                                    class: 'dropdown-item d-flex align-items-center',
                                                    href: 'esp-logout',
                                                    id: 'logoutHeader'
                                                },
                                                children: [
                                                    { type: 'i', props: { class: 'bi-box-arrow-right' } },
                                                    { type: 'span', children: ['Exit'] }
                                                ]
                                            }
                                        ]
                                    }
                                ]
                            }
                        ]
                    }
                ]
            }
        ]
    });
    contenedor.appendChild(nav);//
}
// createsidebar barra de opciones lateral
export function createSidebarNav(list) {
    const contenedor = document.querySelector('.sidebar-nav');
    list.forEach(element => {
        if (element.collapsed) {
            //definir url
            let ulContainer = {
                type: 'ul',
                props: {
                    class: 'nav-container collapse',
                    bsParent: '#sidebar-nav',
                    id: `${element.name}-nav` // usados para asignar variables
                },
                children: []
            }
            // recorrer cada collapsed true
            element.link.forEach(link => {
                const li = {
                    type: 'li',
                    children: [
                        {
                            type: 'a',
                            props: { class: url[3] === `/` + link.url ? 'active' : '', href: link.url },
                            children: [
                                {
                                    type: 'i',
                                    props: { class: 'bi-asterisk' }
                                },
                                {
                                    type: 'span',
                                    children: [link.text]
                                }

                            ]
                        }
                    ]
                }
                if (ulContainer.props.class != 'nav-content collapse show')
                    ulContainer.props.class = url[3] === `/` + link.url ? 'nav-content collapse show' : 'nav-content collapse';
                ulContainer.children.push(li);// todos los li que aparezcan se colocan en ulcontainer
                //console.log(li);

            });
            // creando li
            const li = builder({
                type: 'li',
                props: { class: 'nav-item' },
                children: [{
                    type: 'a',
                    props: {
                        class: ulContainer.props.class === 'nav-content collapse show' ? 'nav-link' : 'nav-link collapsed',
                        bsTarget: `#${element.name}-nav`,
                        bsToggle: 'collapse',
                        href: '#'
                    },
                    children: [// vienen del archivo template
                        { type: 'i', props: { class: `bi-${element.icon}` } },
                        { type: 'span', props: {}, children: [element.name] },
                        { type: 'i', props: { class: 'bi-chevron-down ms-auto' } }//bi-chevron-down ms-auto flecha
                    ]
                },
                    ulContainer
                ]
            });
            contenedor.appendChild(li);
        }
        else {

            const li = builder({
                type: 'li',
                props: { class: 'nav-item' },
                children: [
                    {
                        type: 'a',
                        props: {
                            class: url[3] === `/` + element.url ? 'nav-link' : 'nav-link collapsed',
                            href: element.url === '' ? '/' : element.url,
                            id: element.name === 'Exit' ? 'logout' : ''
                        },
                        children: [
                            {
                                type: 'i',
                                props: { class: `bi-${element.icon}` }
                            },
                            {
                                type: 'span',
                                children: [element.name]
                            }
                        ]

                    }
                ]
            });
            contenedor.appendChild(li);
        }

    })
}

//funcion para crear breadcrum las rutas inicio/dashboard
export function createBreadCrumb(title, funcion, link) {// titulo, funcion y link
    const contenedor = document.querySelector('.pagetitle');
    const h1 = builder(
        {
            type: 'h1',
            children: [title]

        });
    //nav
    const nav = builder(
        {
            type: 'nav',
            children: [
                {
                    type: 'ol',
                    props: { class: 'breadcrumb' },
                    children: [
                        {
                            type: 'li',
                            props: { class: 'breadcrumb-item' },
                            children: [
                                {
                                    type: 'a',
                                    props: { href: '/' },
                                    children: ['Inicio']
                                }]
                        },
                        {
                            type: 'li',
                            props: { class: 'breadcrumb-item' },
                            children: [funcion]
                        },
                        {
                            type: 'li',
                            props: { class: 'breadcrumb-item active' },
                            children: [link]



                        }
                    ]
                }
            ]
        });
    contenedor.appendChild(h1);
    contenedor.appendChild(nav)
}

// POO PARA LA API SERVICE
export class ApiService {
    //constructor, se inicializa cuando se invoque la clase
    constructor(path, data) {// path es para formar url y data cuando se hagan las peticiones
        this.path = path;
        this.data = data;
    }
    // metodo para ejecutar peticiones get de la informacion 
    async getApiData() {
        try {
            const urlGet = `http://${host}/api/${this.path}`;// string template, estructura de url
            const response = await fetch(urlGet,
                {
                    method: 'GET',
                    headers: {
                        'Accept': 'application/json'
                    }
                }
            );
            const json = await response.json();
            return await json;
        }
        catch (error) {
            console.log(error);
        }
    }
}

// Crear card para index
export function createCard(padre, classCard, tittle, icon = 'cpu', id, value, titleSmall,bgraund="none") {
    const contenedor = document.querySelector(padre);
    const CardChildren =
    [
        {
            type: 'div',
            props: { class: 'card-body cta' },
            children: [
                {
                    type: 'h4',
                    props: { class: 'card-title text-start' },
                    children: [`${tittle}`]

                },
                {
                    type: 'div',
                    props: { class: 'd-flex align-items-center' },
                    children: [
                        {
                            type: 'div',
                            props: { class: 'card-icon rounded-circle d-flex align-items-center justify-content-center' },
                            children: [
                                {
                                    type: 'i',
                                    props: { class: `bi-${icon}` },
                                }

                            ]
                        },
                        {
                            type: 'i',
                            props: { class: 'ps-1' },
                            children: [
                                {
                                    type: 'h5',
                                    props: { id: `${id}`, class: 'text-start ps-1' },
                                    children: [`${value}`]
                                },
                                {
                                    type: 'span',
                                    props: { class: 'text-muted small pt-2 ps-0' },
                                    children: [`${titleSmall}`]
                                }

                            ]
                        }

                    ]
                }
            ]
        }
    ];

    if (bgraund !== "none") {
        CardChildren.unshift({
            type: 'img',
            props: { class: 'card-img-bg ¿', src: `${bgraund}`}
        });
    }
    const card = builder({
        type: 'div',
        props: { class: `card info-card ${classCard}`},
        children:CardChildren
    });
    contenedor.appendChild(card);

}
