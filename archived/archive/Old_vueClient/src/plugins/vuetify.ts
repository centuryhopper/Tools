// src/plugins/vuetify.ts
import { createVuetify } from './vuetify';
import 'vuetify/styles'; // Global CSS required by Vuetify

// Import any Vuetify components or directives you want here, e.g.:
import * as components from 'vuetify/components';
import * as directives from 'vuetify/directives';

export default createVuetify({
    components,
    directives,
})
