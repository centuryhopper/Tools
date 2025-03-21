const WEEKDAY_EARNINGS: f32 = 25.0;
const WEEKEND_DAY_EARNINGS: f32 = 50.0;
const FUEL_COST_PER_GALLON: f32 = 3.0;

#[derive(Debug)]
pub struct Earnings {
    pub weekly_earnings: f32,
    pub monthly_earnings: f32,
    pub annual_earnings: f32,
}

impl Earnings {
    // pub fn new() -> Earnings {
    //     Self {
    //         weekly_earnings: 0 as f32,
    //         monthly_earnings: 0 as f32,
    //         annual_earnings: 0 as f32,
    //     }
    // }

    pub fn calculate_net_uber_delivery_earnings(car_mpg: &u32) -> Earnings {
        let weekly_earnings = WEEKDAY_EARNINGS * 5.0 + WEEKEND_DAY_EARNINGS * 2.0;
        let monthly_earnings = weekly_earnings * 4.0;
        let annual_earnings = monthly_earnings * 12.0;

        // 2 hours * 5 weekdays per week * 30 miles (pretend that you drive no more than 30 miles on a
        //   weekday)
        let week_day_mileage = 2.0 * 5.0 * 30.0;
        // 4 hours * 2 weekend days per week * 60 miles (pretend that you drive no more than 60 miles
        //   on a weekend day)
        let weekend_day_mileage = 4.0 * 2.0 * 60.0;
        let weekly_fuel_consumption_in_gallons =
            (week_day_mileage + weekend_day_mileage) / *car_mpg as f32;
        let weekly_fuel_cost = FUEL_COST_PER_GALLON * weekly_fuel_consumption_in_gallons;
        let monthly_fuel_cost = weekly_fuel_cost * 4.0;
        let annual_fuel_cost = monthly_fuel_cost * 12.0;

        Earnings {
            weekly_earnings: weekly_earnings - weekly_fuel_cost,
            monthly_earnings: monthly_earnings - monthly_fuel_cost,
            annual_earnings: annual_earnings - annual_fuel_cost,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_calculate_net_uber_delivery_earnings() {
        let car_mpg = 25;
        let earnings = Earnings::calculate_net_uber_delivery_earnings(&car_mpg);
        assert!(earnings.weekly_earnings > 0.0);
    }
}
