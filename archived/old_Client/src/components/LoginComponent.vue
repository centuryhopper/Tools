<script setup lang="ts">
import { ref } from "vue";
import { useRouter } from "vue-router";
import useCountDown from "../composables/useCountDown";
import { useTheme } from "../stores/theme-store";
import type { LoginModel, LoginPageProps } from "../types/LoginTypes";

const { isDark } = useTheme();

const {
  cooldownSeconds = 10,
  maxAttempts = 5,
  noticeText = "",
  redirectLink = "/",
  onLogin,
} = defineProps<LoginPageProps>();

const model = ref<LoginModel>({
  email: "",
  password: "",
  rememberMe: false,
});

const isPasswordVisible = ref(false);
const loading = ref(false);
const errMsg = ref("");
const attemptsNotice = ref("");
const attempts = ref(0);
const isCoolingDown = ref(false);
const router = useRouter();

const { secondsLeft, startTimer } = useCountDown({
  onComplete: () => {
    isCoolingDown.value = false;
    attempts.value = 0;
    errMsg.value = "";
    attemptsNotice.value = "";
  },
  onTick: () => {
    errMsg.value = `Too many attempts. Please wait ${secondsLeft.value} seconds before trying again.`;
  },
});

const togglePasswordVisibility = () => {
  isPasswordVisible.value = !isPasswordVisible.value;
};

const handleChange = (event: Event) => {
  const target = event.target as HTMLInputElement;
  const { id, value, checked, type } = target;
  (model.value as any)[id] = type === "checkbox" ? checked : value;

  // console.log(model.value.email);
};

const handleSubmit = async () => {
  if (isCoolingDown.value) return;
  loading.value = true;
  attempts.value++;

  const response = await onLogin(model.value);
  loading.value = false;

  if (!response.flag) {
    if (attempts.value >= maxAttempts) {
      isCoolingDown.value = true;
      attemptsNotice.value = "";
      startTimer(cooldownSeconds);
    } else {
      errMsg.value = response.message;
      attemptsNotice.value = `${attempts.value} of ${maxAttempts} attempts`;
    }
    return;
  }

  // console.log('redirecting to', redirectLink);

  router.push(redirectLink);
};
</script>

<template>
  <div class="container p-5">
    <form
      id="login_form"
      @submit.prevent="handleSubmit"
      :class="`card text-white ${isDark ? 'bg-black bg-gradient' : ''}`"
    >
      <h4 :class="`card-header text-${isDark ? 'white' : 'black'}`">Login</h4>
      <div class="card-body">
        <div class="form-group">
          <label for="email" :class="`text-${isDark ? 'white' : 'black'}`">Email</label>
          <input
            id="email"
            type="email"
            class="form-control"
            v-model="model.email"
            required
            placeholder="Enter your email"
            @input="handleChange"
          />
        </div>
        <div class="form-group mt-3">
          <label for="password" :class="`text-${isDark ? 'white' : 'black'}`"
            >Password</label
          >
          <div class="input-group">
            <input
              id="password"
              placeholder="Enter your password"
              :type="isPasswordVisible ? 'text' : 'password'"
              class="form-control"
              v-model="model.password"
              required
              @input="handleChange"
            />
            <div class="input-group-append">
              <button
                type="button"
                class="btn btn-outline-secondary"
                @click="togglePasswordVisibility"
              >
                <i
                  :class="['fa', isPasswordVisible ? 'fa-eye' : 'fa-eye-slash']"
                  aria-hidden="true"
                ></i>
              </button>
            </div>
          </div>
        </div>

        <div class="form-check my-3">
          <input
            id="rememberMe"
            class="form-check-input"
            type="checkbox"
            v-model="model.rememberMe"
            @change="handleChange"
          />
          <label
            :class="`form-check-label text-${isDark ? 'white' : 'black'}`"
            for="rememberMe"
          >
            Remember Me
          </label>
        </div>

        <button
          class="btn btn-primary"
          type="submit"
          :disabled="loading || isCoolingDown"
        >
          <span v-if="loading" class="spinner-border spinner-border-sm mr-1"></span>
          Login
        </button>

        <p v-if="errMsg" class="text-danger mt-3">{{ errMsg }}</p>
        <p v-if="attemptsNotice" class="text-danger">{{ attemptsNotice }}</p>
      </div>
    </form>

    <div v-if="noticeText" class="text-center font-weight-bold mt-3">
      {{ noticeText }}
    </div>
  </div>
</template>

<style scoped>
/* .container {
    max-width: 400px;
    margin: auto;
    padding-top: 40px;
    } */

#login_form input {
  /* color: white; */
  /* background-color: black; */
}
</style>
