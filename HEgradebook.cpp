//Homomorphic encryption, class grade book
#include <FHE.h>
#include <EncryptedArray.h>
#include <NTL/lzz_pXFactoring.h>

#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>

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
	cout << "insert grades seperated by spaces. ex:75 91 100" << endl;
	cout << "Enter q to quit" << endl;
}

//begin main
int main(int argc, char** argv) {
	// Local variables
	string teacher_input;
	int teacher_points;
	int teacher_choice;
	greeting();

	// setup FHE
    setupHELib();
    //host for our plaintext values when they go to get encrypted then insert into grades_vector
    EncryptedArray enc_arr(*context, G);

    // insert loop
	while(true) {
		
		cin >> teacher_input;
		if(teacher_input[0] == 'q') {
			break;
		}
		/*if (grades_vector.size() < 1) {
                cout << "not enough grades inserted" << endl;
        }
        */
            /*
            else {
			    // perform the operation
			    evaluate(teacher_input[0]);
            } //end else
            */


		else {
			cout << "In the add grades to encypt book block" << endl;
			// should be a number "grade", insert it
			// encrypt it first 
            Ctxt& c = *(new Ctxt(*publicKey));
            PlaintextArray p(enc_arr); 
            p.encode(atoi(teacher_input.data()));
            //test if my values were inserted correctly
            //int grade = atoi(teacher_input.data());
            //cout << grade + " grade" << endl;
            enc_arr.encrypt(c, *publicKey, p); 
            
			//grades_vector.push_back(atoi( teacher_input.data() ));
            grades_vector.push_back(c);
		} //end else
		


	} //end insert while loop
	cout <<"start ineractive portion" << endl;
	// operations while loop
	while(true) {
		cout << "d 1- display grades" << endl;
		cout << "a 2- add point value to all grades in book" << endl;
		cout << "s 3- subtract point value to all grades in book" << endl;
		cout << "q 4- quit" << endl;
		
		cin >> teacher_choice;

		//use op1 & op2 for add and subtract operations
		Ctxt *op1, *op2;

		switch(teacher_choice)
		{
			case 1:
			{
				// TODO: decrypt before printing
				//Loop through the grade book
				//enc_arr.decrypt(grades_vector, *secretKey, clear_grades_vector);
				cout << "The " << grades_vector.size() << " grades are: " ;
				for(int i = 0; i < grades_vector.size(); i ++){
					//PlaintextArray p(enc_arr);
					enc_arr.decrypt(grades_vector[i], *secretKey, clear_grades_vector);
    				//enc_arr.decrypt(grades_vector[i], *secretKey, p); //grades_vector is still encrypted data
   					//cout << p.decode(enc_arr) << endl;
   					//p.print(cout);
   					//clear_grades_vector.push_back(p.get());
   					cout << clear_grades_vector[i] << " ";
				}//end for loop
				/*
				//print clear grade book
				for(int i = 0; i < clear_grades_vector.size(); i++)
					cout << clear_grades_vector[i] << " ";
    			cout << endl;
    			*/
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
    			//op2 = new Ctxt(atoi(teacher_input.data()));
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
				cout << "Exiting, As you wish" << endl;
				break;
			}
			default: cout << "Invalid command, Try again" << endl;
		} //end switch
		if(teacher_choice == 4)
			break; 

	} //end while loop

} //end main()


//function to set HE resources
void setupHELib() {
    long p=101;
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