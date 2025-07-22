import { GeneralResponse, LoginDTO, LoginResponse } from "../types";

export interface IAccountService
{
  logout(userId: number): Promise<GeneralResponse>;
  loginAccount(loginDTO: LoginDTO): Promise<LoginResponse>;
}
