#include "Frontend/ASTree.cpp"
#include "Tokenizer/Tokenizer.cpp"
#include "Backend/CodeGenerator.cpp"

int main(int argc, const char* argv[]){
	FILE* code = fopen((argc - 1 >= 1)?(argv[1]):("test.aristotle"), "r");
	TokenizerNS::Tokenizer t(code);
	ASTreeNS::ASTree tree(t.tokens());	
	tree.dump("dump.dot");

	CodeGeneratorNS::CodeGenerator gen(tree);
	gen.write("output.asm");
}
