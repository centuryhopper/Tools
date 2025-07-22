export interface GeneralResponse
{
  flag: boolean;
  message: string;
}

export interface LoginResponse extends GeneralResponse
{
  token?: string;
}

export type LoginDTO = {
  email: string;
  password: string;
  rememberMe: boolean;
};

export type PlaidItemDTO = {
  userid: number;
  accesstoken: string;
  institutionname: string;
  datelinked: Date | null; // or Date | null if parsed
  access_token_iv: string | null | undefined;
  transactions_cursor: string | null | undefined;
};

export type UserDTO = {
  id: number;
  umsUserid: string;
  email: string;
  firstname: string;
  lastname: string;
  datelastlogin: string | null;
  datelastlogout: string | null;
  datecreated: string | null;
  dateretired: string | null;
  //   roles: string[];
};
