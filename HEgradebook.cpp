//Homomorphic encryption, class grade book
//Made by Michael Lockette student project for the University of Colorado Colorado Springs

//include libraries specialized for Homomorphic use
#include <FHE.h>
#include <EncryptedArray.h>
#include <NTL/lzz_pXFactoring.h>

#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>

#include <fstream>
#include <sstream>

using namespace std;

vector<Ctxt> grades_vector;
vector<long> clear_grades_vector;
FHEcontext* context;
FHESecKey* secretKey;
FHEPubKey* publicKey;
ZZX G;

//reference for function below that will init HE resources
void setupHELib();

void greeting() {
	cout << "Welcome to the homomorphic encryption grade book" << endl;
	cout << "insert grades seperated by commas in a csv file named grades.csv" << endl;
	cout << "Could take a few minutes to load csv file" << endl;
}

//begin main
int main(int argc, char** argv) {
	// Local variables
	//read file in stuff
	ifstream infile; //input file
	infile.open("grades.csv", ios::in); //open the file grades.csv
	string line;
	vector<string> tmp; //store string values from csv file
	
	string teacher_input;
	int teacher_points;
	int teacher_choice;
	greeting();

	// setup FHE
    setupHELib();
    //host for our plaintext values when they go to get encrypted then insert into grades_vector
    EncryptedArray enc_arr(*context, G);

	//csv file insert grades loop
	getline(infile, line);
	stringstream linestream (line);
	string cell;
	cout << "Begin while in loop from csv" << endl;
	while(getline(linestream, cell, ',')){ //how I define the comma as a seperating value
		tmp.push_back(cell);
	}//end while

	Ctxt& c = *(new Ctxt(*publicKey));
    PlaintextArray p(enc_arr); 
	for(int i=0; i < tmp.size(); i++){
		//use tmp values into encrypted vector
		cout << "In the add grades to encypt book block" << endl;
		// should be a number "grade", insert it
		// encrypt it first
		p.encode(atoi(tmp[i].c_str())); 
		enc_arr.encrypt(c, *publicKey, p);
		grades_vector.push_back(c);
	}//end for loop	


	cout <<"start ineractive portion" << endl;
	// operations while loop
	while(true) {
		cout << "d 1- display grades" << endl;
		cout << "a 2- add point value to all grades in book" << endl;
		cout << "s 3- subtract point value to all grades in book" << endl;
		cout << "v 4- find class average" << endl;
		cout << "q 5- quit" << endl;
		
		cin >> teacher_choice;

		//use op1 & op2 for add and subtract operations
		Ctxt *op1, *op2;

		switch(teacher_choice)
		{
			case 1:
			{
				// TODO: decrypt before printing
				//Loop through the grade book
				cout << "The " << grades_vector.size() << " grades are: " ;
				for(int i = 0; i < grades_vector.size(); i ++){
					enc_arr.decrypt(grades_vector[i], *secretKey, clear_grades_vector); 
   					cout << clear_grades_vector[i] << " ";
				}//end for loop
				
    			cout << endl;
    			break;
    		}
    		case 2:
    		{
    			cout << "how many points would you like to add to all the grades:" << endl;
    			cin >> teacher_points;
    			//take point input and encrypt with public key
    			Ctxt& c = *(new Ctxt(*publicKey));
            	PlaintextArray p(enc_arr); 
            	p.encode(teacher_points);
            	enc_arr.encrypt(c, *publicKey, p);
            	cout << "Encrypted teacher points" << endl;
    			//op2 = new Ctxt(atoi(teacher_input.data()));
    			//set op2 to "c" the encrypted point value we will add to all grades
            	op2 = new Ctxt(c);

            	//At this point this operation could be done on cloud resources
    			//need to construct loop to iterate through grades_vector and perform operation
    			for(int i = 0; i < grades_vector.size(); i++){	
    			//for(int i = grades_vector.begin(); i !=grades_vector.end(); i ++){
    				cout << "perform operation w/ encrypted data" << endl;
    				op1 = new Ctxt(grades_vector[i]);
				
            		(*op1) += (*op2);
					grades_vector[i]= *op1;

				} //end for loop
				cout << "As you wish" << endl;
				break;
			}
			case 3:
			{
				cout << "how many points would you like to deduct to all the grades:" << endl;
    			cin >> teacher_points;
    			
    			Ctxt& c = *(new Ctxt(*publicKey));
            	PlaintextArray p(enc_arr); 
            	p.encode(teacher_points);
            	enc_arr.encrypt(c, *publicKey, p);
    			//set op2 to "c" the encrypted point value we will add to all grades
            	op2 = new Ctxt(c);

    			//need to construct loop to iterate through grades_vector and perform operation
    			for(int i = 0; i < grades_vector.size(); i++){	
					cout << "perform operation w/ encrypted data" << endl;
    				op1 = new Ctxt(grades_vector[i]);
				
            		(*op1) -= (*op2);
					grades_vector[i]= *op1;
				} //end for loop
				cout << "As you wish" << endl;
				break;
			}
			case 4:
			{
				cout << "This feature is under progress" << endl;
				//need to get size of grades vector and sum...
				int class_size = grades_vector.size();
				Ctxt *c_sum;
				c_sum = new Ctxt(grades_vector[0]);
				cout << "begin sum loop" << endl;
				for(int i = 1; i < grades_vector.size(); i++){
					//i think I gotta make grade_vector[i] a pointer value first
					op1 = new Ctxt(grades_vector[i]);
					(*c_sum) += (*op1);
					cout << "added another value to sum" << endl;
				} //end for loop
				cout << "added up grade book vector" << endl;
				vector<long> average;
				enc_arr.decrypt(*c_sum, *secretKey, average);
				double average_clear_txt = (double)average[0] / (double)class_size; 
   				cout << "The average is " << average_clear_txt << endl;
				
			}
			case 5:
			{
				cout << "Exiting, As you wish" << endl;
				break;
			}
			default: cout << "Invalid command, Try again" << endl;
		} //end switch
		if(teacher_choice == 5)
			break; 

	} //end while loop

} //end main()


//function to set HE resources
void setupHELib() {
    //set this value so large so it can handle average operation
    long p=1013;
    long r=1;
    long L=8;
    long c=2;
    long k=80;
    long s=0;
    long d=0;
    long w=64;
    long m = FindM(k, L, c, p, d, s, 0);
    
    context = new FHEcontext(m,p,r);
    buildModChain(*context, L, c);
    G = context->alMod.getFactorsOverZZ()[0];

    secretKey = new FHESecKey(*context);
    publicKey = secretKey;

   secretKey->GenSecKey(w); 
   addSome1DMatrices(*secretKey); // compute key-switching matrices that we need
} //end setupHELib()