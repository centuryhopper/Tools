import { jwtDecode } from "jwt-decode";
import { defineStore } from "pinia";
import { computed, ref } from "vue";

interface Claims
{
  exp?: number;
  [key: string]: any;
}

export const authStore = defineStore("auth", () =>
{
  const token = ref<string | undefined>(undefined);
  const claims = ref<Claims | undefined>(undefined);

  // Load token on store initialization
  function loadToken()
  {
    const storedToken =
      localStorage.getItem("token") ?? sessionStorage.getItem("token");
    if (storedToken)
    {
      const decoded = jwtDecode(storedToken);
      const isExpired = decoded?.exp && Date.now() >= decoded.exp * 1000;
      if (!isExpired)
      {
        token.value = storedToken;
        claims.value = decoded;
      } else
      {
        localStorage.removeItem("token");
        sessionStorage.removeItem("token");
      }
    }
  }

  // Call loadToken immediately when store is created
  loadToken();

  function login(newToken: string, rememberMe: boolean)
  {
    // const decoded = parseJwt(newToken);
    const decoded = jwtDecode(newToken);
    // console.log('decoded:',decoded);
    if (rememberMe)
    {
      localStorage.setItem("token", newToken);
    } else
    {
      sessionStorage.setItem("token", newToken);
    }
    token.value = newToken;
    claims.value = decoded;
  }

  function logout()
  {
    localStorage.removeItem("token");
    sessionStorage.removeItem("token");
    token.value = undefined;
    claims.value = undefined;
  }

  // TODO: comment this line out when running from the backend
  // This is just for debugging purposes
  // const isAuthenticated = false
  const isAuthenticated = computed(() => !!token.value);

  return {
    token,
    claims,
    login,
    logout,
    isAuthenticated,
  };
});
