export type LoginModel = {
  email: string;
  password: string;
  rememberMe: boolean;
};

export type LoginResponse = {
  flag: boolean;
  message: string;
};

export type LoginPageProps = {
  cooldownSeconds?: number;
  maxAttempts?: number;
  noticeText?: string;
  redirectLink?: string;
  loginCallback: (model: LoginModel) => Promise<LoginResponse>;
};
