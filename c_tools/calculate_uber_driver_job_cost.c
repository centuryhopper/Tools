#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>


typedef struct Earnings
{
    float weeklyEarnings;
    float monthlyEarnings;
    float annualEarnings;
} Earnings;


// assume we make these amounts in dollars per weekday or weekend day
const float weekdayEarnings = 25.0f;
const float weekendDayEarnings = 50.0f;

// assume 3 dollars per gallon of fuel
const float fuelCostPerGallon = 3.0f;

// gas car
// const int carMpg = 25;

Earnings calculate_net_uber_delivery_earnings(int carMpg)
{
    float weeklyEarnings = weekdayEarnings * 5 + weekendDayEarnings * 2;
    float monthlyEarnings = weeklyEarnings * 4;
    float annualEarnings = monthlyEarnings * 12;

    // calculate mileage
    // assume you work 2 hours each weekday after your full time job and around 4 hours each weekend day
    // assume up to 30 miles per day on weekdays
    // assume up to 60 miles per day on weekends
    // two hours of operating vehicle multipled by five weekdays per week multiplied by 30 miles per day
    float weekDayMileage = 2 * 5 * 30;
    // four hours of operating vehicle multipled by 2 weekend days per week multiplied by 60 miles per day
    float weekendDayMileage = 4 * 2 * 60;
    float weeklyFuelConsumptionInGallons = (weekDayMileage + weekendDayMileage) / carMpg;
    float weeklyFuelCost = fuelCostPerGallon * weeklyFuelConsumptionInGallons;
    float monthlyFuelCost = weeklyFuelCost * 4;
    float annualFuelCost = monthlyFuelCost * 12;

    printf("weekly fuel cost: $%f, weekly earnings: $%f\n", weeklyFuelCost, weeklyEarnings);
    printf("monthly fuel cost: $%f, monthly earnings: $%f\n", monthlyFuelCost, monthlyEarnings);
    printf("annual fuel cost: $%f, annual earnings: $%f\n", annualFuelCost, annualEarnings);
    printf("total mileage per week: %f miles\n", weekDayMileage + weekendDayMileage);
    Earnings earnings = {weeklyEarnings - weeklyFuelCost, monthlyEarnings - monthlyFuelCost, annualEarnings - annualFuelCost};

    return earnings;
}


int main(int argc, char *argv[])
{
     // Check if an argument is provided
    if (argc != 2) {
        fprintf(stderr, "Please enter your car's mileage-per-gallon (mpg).\n");
        fprintf(stderr, "Usage: %s <number>\n", argv[0]);
        return EXIT_FAILURE; // Exit with an error code
    }

    /*
        strtol allows better error handling than atoi because it accepts an endptr pointer (for tracking where the conversion stops) and sets errno if it encounters a range error.
    */
    char *endptr; // Pointer for strtol
    errno = 0; // Reset errno before conversion
    long value = strtol(argv[1], &endptr, 10); // Convert string to long

    // Check for conversion errors
    if (errno != 0) {
        perror("Conversion error");
        return EXIT_FAILURE; // Exit with an error code
    }

    // Check if the entire string was converted
    if (endptr == argv[1]) {
        fprintf(stderr, "No digits were found in the input.\n");
        return EXIT_FAILURE; // Exit with an error code
    }

    // Check if the number is out of the int range
    if (value < INT_MIN || value > INT_MAX) {
        fprintf(stderr, "Number out of range: %ld\n", value);
        return EXIT_FAILURE; // Exit with an error code
    }

    Earnings earnings = calculate_net_uber_delivery_earnings((int) value);

    printf("(weekly net earnings: $%f, monthly net earnings: $%f, annual net earnings:  $%f)\n", earnings.weeklyEarnings, earnings.monthlyEarnings, earnings.annualEarnings);
    return EXIT_SUCCESS;
}




