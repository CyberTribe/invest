// this programm is meant for basic analysis of investment.
// it takes cash flows as input and outputs such parameters as:
// NPV - total present value of the project.
// i_profit - index of profit of the project
// p_profit = period before project starts to give profit
// ea - equivalent annuitet
// irr = inner return rate. Estimation of such percent rate at which project generate enough money to payback itself, but doesn't give any profit.
// irr is the percent rate at which npv equals zero
// mirr = modified inner return rate. modification of irr, due to irr being sometimes too optimistic.
// TODO: move all functions into a class, use iterators, instead of indexes, and also get rid of global vectors.

#include <iostream>
#include <vector>
#include <cmath>
#define ITER 10 // We need only 10 iterations for IRR approximation
using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::vector;

/*-----------------------------------------------------*/

// global vestors for now
// TODO:need to replace with iterators later, and make them private.
vector <long double> g_invest; // Global vector for investment amounts
vector <long double> g_d_invest; // Global vector for discounted investment amounts
vector <long double> g_revenue; // Global vector for revenue amounts
vector <long double> g_d_revenue; // Global vector for discounted revenue amounts
vector <long double> g_d_total; // Global vector for total cash flow amounts
vector <long double> g_d_float; // Global vector for revenue without investment cash flow
vector <long double> g_k_discont; // Global vector for coefficients of discount
vector <long double> g_k_infl; // Global vector for coefficients of inflating

// This function is used for data input, also it initializes the global vectors. Arguments are self descriptive.
void data_input ( int &length, long double &discont )
{
  // Read length of project.
  bool error = false; // variable for checking input
  do
  {
	error = false;
	cout << "Input the length of project" << endl;
	cin >> length;

	// Length can't be less then or equal zero
	if ( length <= 0 ) {
	  cerr << "Wrong length of project" << endl;
	  cerr << "The length of project must be greater then 0" << endl;
	  error = true;
	}
  }
  while (error == true);

  // Initialization of vectors.
  g_invest.resize (length+1);
  g_revenue.resize (length+1);
  g_d_invest.resize (length+1);
  g_d_revenue.resize (length+1);
  g_d_float.resize (length+1);
  g_d_total.resize (length+1);
  g_k_discont.resize (length+1);
  g_k_infl.resize (length+1);

  // Read the discount rate of the project.
  do
  {
	cout << "Input the percent rate of discont" << endl;
	cin >> discont;
	error = false;

	// Now convert from percents to 0.x
	discont /= 100;

	// The discont rate can't be less then zero, and doubtfull will ever be more then 1.
	if ( discont <= 0 || discont >= 1 ) {
	  cerr << "Wrong rate of discont" << endl;
	  cerr << "Rate of discont must be between 0 and 100 percents" << endl;
	  error = true;
	}
  }
  while (error == true);

  // Now we will read the values for investment and revebue cash flows.
  for ( int i = 0; i <= length; ++i ) {
	cout << "Input the amount of investment in " << i << " year of project." << endl;
    // amount of investment
    if ( i == 0 ) {
	  do {
	  cout << "Investment in the 0th year of project can't be less or equal 0." << endl;
	  cin >> g_invest[i];
	  }
	  while (g_invest[i] <= 0);
	} else {
	  cin >> g_invest[i];
	}
    // amount of income
    cout << "Input the amount of revenue in " << i << " year of project." << endl;
    cin >> g_revenue[i];
  }
}
/*-----------------------------------------------------*/
// This function is used to make some basic calculations, used later.
// First argument is discont rate, second is length of project. We use copies of arguments here, as we don't need to change them.
// TODO:replace vectors with iterators.
// Also it calculates NPV, and returns its value.
long double npv ( int length, long double discont )
{
  //First some local variables
  long double npv_value = 0;

  for ( int i = 0; i <= length; ++i ) {

    // calculating the coefficients of discount. Used in most other functions.
    i == 0 ? g_k_discont[i] = 1 : g_k_discont[i] = g_k_discont[i-1] / ( 1 + ( discont ) );

	// calculating the coefficients of inflation. Used in mirr calculation.
    g_k_infl[i]=pow( 1 + discont , length - i );

	// calculating the discounted amount of investment (investment cash flow)
    g_d_invest[i]=g_invest[i]*g_k_discont[i];

	// calcualting the discounted amount of revenue (operational cash flow)
    g_d_revenue[i]=g_revenue[i]*g_k_discont[i];

	// calculating the discounted cash flow without investment
    g_d_float[i]=g_d_revenue[i] - g_d_invest[i];

	// calculating total growing up cash flow.
    i == 0 ? g_d_total[i] = g_d_float[i] : g_d_total[i] = g_d_total[i-1] + g_d_float[i];

  }
  // Return NPV values (the last element of vector g_d_total is NPV).
  return g_d_total[length];
}
/*-----------------------------------------------------*/
// This function is used for calculating the profit index. It shows how much money you get from the project for every dollar/rouble/watever else you invest into it.
// the two common variables are used here. profit index is used as a return value.
// TODO: as everywhere, use iterators instead of indexes and global vectors.
long double profit_index ( int length, long double discont )
{
  //some local variables declaration.
  //d_invest_sum - discounted sum of investment
  long double d_invest_sum = 0;

  //d_revenue_sum - discounted sum of revenue
  long double d_revenue_sum = 0;

  // calculating index of profit
  for ( int i = 0 ; i <= length ; ++i ) {
    d_invest_sum += g_d_invest[i];
    d_revenue_sum += g_d_revenue[i];
  }

  return ( d_revenue_sum / d_invest_sum );
}
/*-----------------------------------------------------*/
// This function is used for calculating the period before projects starts to give profit, not only payback the investment.
// The only argument is the length of the project. Others are taken from global vectors. (need to get rid of them and replace with iterators).
// the profit period is given as a return value.
float profit_period ( int length )
{
  // Some local variables
  float p_profit = 0; // type is float as we don't need more then 2 signs after the comma.

  // calculating the period for getting profit
  for ( int i = 0; i <= length; ++i ) {
	// If total cash flow is more then zero, then we take the previous amount of total cash flow, and divide it to the amount of cash flow in that year.
    if ( g_d_total[i] >= 0 ) {
      p_profit = ( i - 1 ) + ( - g_d_total[i-1] / g_d_float[i] );
      }
  }

  return p_profit;
}
/*-----------------------------------------------------*/
// This function is used for calculating equivalent annuitet.
// It accepts the percent rate and the length of the project as it's arguments.
// EA is used for comparing two diffrent projects with diffrent periods of realization.
// EA is given as a return value of this function.
long double ea_calc (int length, long double discont)
{
  // calculating equivalent annuitet
  return ( npv ( length, discont ) / ( ( 1 - g_k_discont[length] ) / ( discont ) ) );
}
/*-----------------------------------------------------*/
// This function calculates MIRR, the modified internal rate of return (see MIRR page on the wikipedia for details).
// MIRR is given as a return value.
// To calculate MIRR it uses some global vectors (!sic), local variables and length of the project as an argument of the function.
long double mirr_calc ( double length )
{
  // This are some local variables, used to calculate MIRR.
  double cfo = 0; // Operational cash flow
  double cfi = 0; // Investment cash flow

  for ( int j = 0; j <= length; j++ ) {
	cfo += g_revenue[j] * g_k_infl[j];
	cfi += g_invest[j] * g_k_discont[j];
  }

  return ( pow ( ( cfo / cfi ), 1 / length ) - 1 );
}
/*-----------------------------------------------------*/
// IRR calculation - the most complex thing in that small programm.
// There's no formula for calculating IRR, so we need to make an estimate of it.
// Usually 10 iterations is enough for getting very good estimates of IRR.
// As it's arguments this function accepts the percent rate of the project.
// Other values are taken from global vectors.
// Attention. after this function the contents of global vectors is replaced with other values, used to make an estimate.
// IRR is given as a return value of this function.
long double irr_calc ( int length, long double discont )
{
  //To start the estimation cycle using formula suggesten in the wiki (see http://en.wikipedia.org/wiki/Internal_rate_of_return )
  // we need two pairs of NPV and percent rate values. We get them here.
  // Here we use 2 local vectors, as we don't need them anywhere outside of this function.
  vector <long double> irr; // Used to store IRR values during approximation
  vector <long double> npv_irr; // Used to store NPV values during approximation

  // Calculating and storing first 2 pairs of values
  irr.push_back ( discont );
  npv_irr.push_back ( npv ( length, discont ) );
  irr.push_back ( discont + 0.05 );
  npv_irr.push_back ( npv ( length, discont + 0.05 ) );

  // Now we can estimate IRR using some mathematical method (see link to wiki).
  // 10 iterations here as it is usually enough to estimate IRR value.
  for ( int j = 1; j <= ITER; ++j ) {
    irr.push_back ( ( irr[j] - npv_irr[j] * ( ( irr[j] - irr[j-1] ) / ( npv_irr[j] - npv_irr[j-1] ) ) ) );
    npv_irr.push_back ( npv ( length, irr[j+1] ) );
  }
  return irr[ITER];
}
/*-----------------------------------------------------*/
int main ()
{
  //percent = the percent used for calculating the koeefficients of discount
  //Usually represents the alternative profitability
  long double percent = 0;

  // period = the length of the project
  int period = 0;

  // again and error - decide if we want to analyse another project and check the status of input
  char again = 'N';
  bool error = false;

  do
  {
	//first we need to input the data.
	data_input ( period , percent );


	// Here we calculate and output NPV of the project
	cout << "NPV of this project = " << npv ( period , percent ) << endl;

	// Here we calculate and output the index of profit of this project
	cout << "Index of profit of this project = " << profit_index ( period , percent ) << endl;

	// Here we calculate and output the period for getting profit out of this project
	cout << "Period of getting profit from this project = " << profit_period ( period ) << endl;

	// Here we calculate and output equivalent annuitet
	cout << "Equivalent Annuitet = " << ea_calc ( period , percent ) << endl;

	// Guess what? Here we calculate and output MIRR. Here we use convesrion of int into double type.
	cout << "MIRR = " << mirr_calc ( period ) << endl;

	// The last part of calculation - IRR.
	cout << "IRR approximately = " << irr_calc ( period , percent ) << endl;

	// If we got here - means we counted everything and can return success.
	cout << endl;
	cout << endl;
	do
	{
	  cout << "Do you want to analyse another project? [Y/N]" << endl;
	  cin >> again;
	  switch (again) {
		case 'Y' :
		  error = false;
		  break;
		case 'N' :
		  error = false;
		  break;
		default :
		  error = true;
		  cout << "Sorry, i didn't understand the answer, please answer Y or N" << endl;
		  break;
	  }
	}
	while ( error == true );
  }
  while ( again == 'Y' );
  return 0;
}
/*-----------------------------------------------------*/