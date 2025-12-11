import 'bootstrap'; // imports JavaScript behavior for dropdowns, navbar toggles, etc.
import 'bootstrap/dist/css/bootstrap.min.css';
import { Chart, registerables } from 'chart.js';
import { createPinia } from "pinia";
import PrimeVue from 'primevue/config';
import 'sweetalert2/dist/sweetalert2.min.css';
import { createApp } from "vue";
import App from "./App.vue";
import router from "./router";
import "./style.css";


import { useTheme } from "./stores/theme-store";
const themeStore = useTheme();
document.documentElement.setAttribute(
    "data-theme",
    themeStore.isDark ? "dark" : "light"
);


Chart.register(...registerables)

const app = createApp(App);
app.use(createPinia());
app.use(router);
app.use(PrimeVue);


app.mount("#app");
