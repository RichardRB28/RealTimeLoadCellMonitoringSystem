"use strict"; // para uso estricto de var, const, let para variables
import { url,createHeader, createSidebarNav} from "./scripts.js";
import { sidebar } from './template.js';
import { iniciarIndex } from "./index.js";
console.log(url);

switch(url[3]){  // /index.html o '/' o 'esp-admin'
    case '/':
        createHeader();
        createSidebarNav(sidebar);
        document.addEventListener('DOMContentLoaded',iniciarIndex)// cuando todo html se haya cargado inicia la funcion index//
        break;
    default:
        break;
}
/* Easy selector helper function
*/
const select = (el, all = false) => {
    el = el.trim()
    if (all) {
        return [...document.querySelectorAll(el)]
    } else {
        return document.querySelector(el)
    }
}

/* Easy event listener function
*/
const on = (type, el, listener, all = false) => {
    if (all) {
        select(el, all).forEach(e => e.addEventListener(type, listener))
    } else {
        select(el, all).addEventListener(type, listener)
    }
}
/**
* Easy on scroll event listener 
*/
const onscroll = (el, listener) => {
    el.addEventListener('scroll', listener)
}

/**
* Back to top button
*/
let backtotop = select('.back-to-top')
if (backtotop) {
    const toggleBacktotop = () => {
        if (window.scrollY > 100) {
            backtotop.classList.add('active')
        } else {
            backtotop.classList.remove('active')
        }
    }
    window.addEventListener('load', toggleBacktotop)
    onscroll(document, toggleBacktotop)
}
/**
 * Sidebar toggle
 */
if (select('.toggle-sidebar-btn')) {
    on('click', '.toggle-sidebar-btn', function (e) {
        select('body').classList.toggle('toggle-sidebar')
    })
}
