#!/home/leo_zhang/miniconda3/envs/web_auto/bin/python

import sys
from typing import Callable
import math

COMPOUND_INTEREST : Callable[[float, float, float, float], float] = lambda p, r, n, t: p * math.pow((1 + (r/n)), n*t)
'''
Compound interest formula
    p: Initial principal (starting amount)
    r: Annual interest rate (e.g. average return of the S&P 500 is anywhere from 7% - 10%)
    n: Number of times interest is compounded per year (typically 1 for index funds)
    t: Number of years invested
'''

FUTURE_VALUE_OF_ANNUITY: Callable[[float, float, float, float], float] = lambda p, r, n, t: p * (math.pow((1 + (r/n)), n*t) - 1) / (r/n)
'''
Future value of an annuity formula
    p: Monthly contribution amount
    r: Annual interest rate (e.g. average return of the S&P 500 is anywhere from 7% - 10%)
    n: Number of contributions per year (12 if monthly for example)
    t: Number of years invested
    
'''

# Total value of investment after t years is: Total = COMPOUND_INTEREST + FUTURE_VALUE_OF_ANNUITY


def calculate_wealth_by_year(current_wealth, rate_of_return, monthly_savings, years):
    total_savings = current_wealth
    for year in range(1, years + 1):
        interest = total_savings * (rate_of_return / 100)
        total_savings += interest + (monthly_savings * 12)
        print(f"Year {year}: Total wealth = {total_savings:.2f}")
    return 0


def calculate_years_till_freedom(
    current_wealth, rate_of_return, monthly_savings, target_wealth
):
    total_savings = current_wealth
    years_to_freedom = 0
    while True:
        years_to_freedom += 1
        interest = total_savings * (rate_of_return / 100)
        total_savings += interest + (monthly_savings * 12)
        if total_savings > target_wealth:
            print(
                f"You will reach financial freedom in {years_to_freedom} years! Keep grinding!! "
            )
            return 0

def try_parse(val: str, default_val: float) -> float:
    try:
        x = float(val)
        if x < 0:
            raise Exception("You can only enter non-negative numbers")
        return x
    except Exception as e:
        return default_val

def main():
    cmpi_p = try_parse(input('Please enter an inital investment dollar amount (default is $10,000): '), 10_000)
    # print(cmpi_p)
    cmpi_r = try_parse(input('Please enter the average rate of return (default is 0.07): '), 0.07)
    cmpi_n = try_parse(input('Please enter the number of times the investment is compounded per year (default is 1): '), 1) # typically 1 for index funds
    cmpi_t = try_parse(input('Please enter the number of years you would like to stay in this investment (default is 20): '), 20)
    fv_p = try_parse(input('Please enter the dollar amount you would like to make per contribution (default is $1000): '), 1000)
    fv_r = cmpi_r # annual rate of return
    fv_n = try_parse(input('Please enter the number of contributions you would like to make to this investment per year (default is 12): '), 12) # number of contributions per year
    fv_t = cmpi_t
    total = COMPOUND_INTEREST(cmpi_p, cmpi_r, cmpi_n, cmpi_t) + FUTURE_VALUE_OF_ANNUITY(fv_p, fv_r, fv_n, fv_t)
    print(total)
    # prog = input("Which program would you like to run? Type 'returns' or 'freedom' ")
    # try:
    #     current_wealth = float(input("Enter your current wealth: "))
    #     rate_of_return = float(input("Enter estimated rate of return (%): "))
    #     monthly_savings = float(input("Enter how much you can save per month: "))
    # except ValueError:
    #     print("Invalid input. You must only enter numbers, dumbass!! ")
    #     sys.exit()
    # if prog == "returns":
    #     years = int(input("Enter investment period in years: "))
    #     calculate_wealth_by_year(current_wealth, rate_of_return, monthly_savings, years)
    # elif prog == "freedom":
    #     try:
    #         target_wealth = float(
    #             input("How much money do you need to be financially free? ")
    #         )
    #     except ValueError:
    #         print("Invalid input. You must only enter numbers, dumbass!! ")
    #         sys.exit()
    #     calculate_years_till_freedom(
    #         current_wealth, rate_of_return, monthly_savings, target_wealth
    #     )
    # else:
    #     print("Invalid input. Type 'returns' or 'freedom'")

if __name__ == "__main__":
    main()
