import { GeneralResponse, PlaidItemDTO } from "../types";

export interface IPlaidItemService
{
  getAccessTokensAsync(userId: number): Promise<PlaidItemDTO[]>;
  storePlaidItemAsync(dto?: PlaidItemDTO): Promise<GeneralResponse>;
  getPlaidItemAsync(
    userId: number,
    institution_name: string
  ): Promise<PlaidItemDTO | undefined>;
  editPlaidItemAsync(dto?: PlaidItemDTO): Promise<GeneralResponse>;
}
