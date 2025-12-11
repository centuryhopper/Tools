
export const randomHexColor = () => '#' + Math.floor(Math.random() * 0xffffff).toString(16).padStart(6, '0');

export const API_BASE_URL = 'https://financeapp-production-8b7b.up.railway.app/' // comment this line out if testing locally
// export const API_BASE_URL = 'http://localhost:5003/' // uncomment this line if testing locally