// cosas comunes oara reutilizar en el proyecto, si se quiere agregar uno nuevo se hace desde esta plantilla a la barra de navegación lateral
export const sidebar=[// lista de objetos (array objects)
    {
        collapsed: false,
        name: 'Dashboard',
        icon: 'house-fill',
        url: '',
        link:[]
    },
    {
        collapsed: true,
        name: 'Connections',
        icon: 'broadcast-pin',
        url: '',
        link:[
            {
                url: 'esp-wifi',
                text: 'WiFi'
            },
            {
                url: 'esp-mqtt',// url manejado para esp32 cuando se solicite re4sponde con este link
                text: 'MQTT'
            }
        ]
    },
    {
        collapsed: true,
        name: 'Device',
        icon: 'sliders',
        url: '',
        link:[
            {
                url: 'esp-restore',
                text: 'Restore to factory'
            },
            {
                url: 'esp-restart',
                text: 'Reboot'
            },
            {
                url: 'esp-device',
                text: 'Settings'
            }
        ]
    },
    {
        collapsed: false,
        name: 'Profile',
        icon: 'person-fill',
        url: 'esp-admin',
        link:[]
    },
    {
        collapsed: false,
        name: 'Exit',
        icon: 'box-arrow-right',
        url: 'esp-logout',
        link:[]
    }
]