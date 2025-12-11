import { onMounted, ref } from "vue";

// dark by default
const isDark = ref(true);

export function useTheme()
{
  const toggleTheme = () =>
  {
    isDark.value = !isDark.value;
    setThemeAttribute();
  };

  const setThemeAttribute = () =>
  {
    const theme = isDark.value ? "dark" : "light";
    // document.documentElement.setAttribute("data-theme", theme); // for custom CSS
    document.documentElement.setAttribute("data-bs-theme", theme); // for Bootstrap 5.3+
    localStorage.setItem("theme", theme);
  };

  onMounted(() =>
  {
    const stored = localStorage.getItem("theme");
    isDark.value = stored === "dark";
    setThemeAttribute();
  });

  // watchEffect(() =>
  // {
  //   console.log('watch effect called');
  //   const theme = isDark.value ? "dark" : "light";
  //   localStorage.setItem("theme", theme);
  //   setThemeAttribute();
  // });

  return { isDark, toggleTheme };
}
