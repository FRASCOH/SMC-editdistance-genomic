#include "ed_Matrix.h"
#include "../../../abycore/circuit/booleancircuits.h"
#include "../../../abycore/sharing/sharing.h"

int32_t test_ed_Matrix_circuit(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
		uint32_t bitlen, uint32_t nthreads, e_mt_gen_alg mt_alg, e_sharing sharing) {

	/**
		Step 1: Create the ABYParty object which defines the basis of all the
		 	 	operations which are happening.	Operations performed are on the
		 	 	basis of the role played by this object.
	*/
	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg);


	/**
		Step 2: Get to know all the sharing types available in the program.
	*/

	std::vector<Sharing*>& sharings = party->GetSharings();

	/**
		Step 3: Create the circuit object on the basis of the sharing type
				being inputed.
	*/
	Circuit* circ = sharings[sharing]->GetCircuitBuildRoutine();


	/**
		Step 4: Creating the share objects - s_alice_money, s_bob_money which
				is used as input to the computation function. Also s_out
				which stores the output.
	*/

	share *s_alice_money, *s_bob_money, *s_out;

	/**
		Step 5: Initialize Alice's and Bob's money with random values.
				Both parties use the same seed, to be able to verify the
				result. In a real example each party would only supply
				one input value.
	*/

	uint32_t alice_money, bob_money, output;
	srand(time(NULL));
	alice_money = rand();
	bob_money = rand();

	/**
		Step 6: Copy the randomly generated money into the respective
				share objects using the circuit object method PutINGate()
				for my inputs and PutDummyINGate() for the other parties input.
				Also mention who is sharing the object.
	*/
	//s_alice_money = circ->PutINGate(alice_money, bitlen, CLIENT);
	//s_bob_money = circ->PutINGate(bob_money, bitlen, SERVER);
	if(role == SERVER) {
		s_alice_money = circ->PutDummyINGate(bitlen);
		s_bob_money = circ->PutINGate(bob_money, bitlen, SERVER);
	} else { //role == CLIENT
		s_alice_money = circ->PutINGate(alice_money, bitlen, CLIENT);
		s_bob_money = circ->PutDummyINGate(bitlen);
	}

	/**
		Step 7: Call the build method for building the circuit for the
				problem by passing the shared objects and circuit object.
				Don't forget to type cast the circuit object to type of share
	*/

	s_out = BuildEdMatrixCircuit(s_alice_money, s_bob_money,
			(BooleanCircuit*) circ);

	/**
		Step 8: Modify the output receiver based on the role played by
				the server and the client. This step writes the output to the
				shared output object based on the role.
	*/
	s_out = circ->PutOUTGate(s_out, ALL);

	/**
		Step 9: Executing the circuit using the ABYParty object evaluate the
				problem.
	*/
	party->ExecCircuit();

	/**
		Step 10:Type casting the value to 32 bit unsigned integer for output.
	*/
	output = s_out->get_clear_value<uint32_t>();

	std::cout << "Testing Edit Distance Matrix in: " << get_sharing_name(sharing)
				<< " sharing: " << std::endl;
	std::cout << "\nAlice Money:\t" << alice_money;
	std::cout << "\nBob Money:\t" << bob_money;
	std::cout << "\nCircuit Result:\t" << (output ? ALICE : BOB);
	std::cout << "\nVerify Result: \t" << ((alice_money > bob_money) ? ALICE : BOB)
				<< "\n";

	delete party;
	return 0;
}

share* BuildEdMatrixCircuit(share *s_alice, share *s_bob,
		BooleanCircuit *bc) {

	share* out;

	/** Calling the greater than equal function in the Boolean circuit class.*/
	out = bc->PutGTGate(s_alice, s_bob);

	return out;
}
