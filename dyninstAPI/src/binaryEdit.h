/*
 * Copyright (c) 1996-2004 Barton P. Miller
 * 
 * We provide the Paradyn Parallel Performance Tools (below
 * described as "Paradyn") on an AS IS basis, and do not warrant its
 * validity or performance.  We reserve the right to update, modify,
 * or discontinue this software at any time.  We shall have no
 * obligation to supply such updates or modifications or any other
 * form of support to you.
 * 
 * This license is for research uses.  For such uses, there is no
 * charge. We define "research use" to mean you may freely use it
 * inside your organization for whatever purposes you see fit. But you
 * may not re-distribute Paradyn or parts of Paradyn, in any form
 * source or binary (including derivatives), electronic or otherwise,
 * to any other organization or entity without our permission.
 * 
 * (for other uses, please contact us at paradyn@cs.wisc.edu)
 * 
 * All warranties, including without limitation, any warranty of
 * merchantability or fitness for a particular purpose, are hereby
 * excluded.
 * 
 * By your use of Paradyn, you understand and agree that we (or any
 * other person or entity with proprietary rights in Paradyn) are
 * under no obligation to provide either maintenance services,
 * update services, notices of latent defects, or correction of
 * defects for Paradyn.
 * 
 * Even if advised of the possibility of such damages, under no
 * circumstances shall we (or any other person or entity with
 * proprietary rights in the software licensed hereunder) be liable
 * to you or any third party for direct, indirect, or consequential
 * damages of any character regardless of type of action, including,
 * without limitation, loss of profits, loss of use, loss of good
 * will, or computer failure or malfunction.  You agree to indemnify
 * us (and any other person or entity with proprietary rights in the
 * software licensed hereunder) for any and all liability it may
 * incur to third parties resulting from your use of Paradyn.
 */

// $Id: binaryEdit.h,v 1.15 2008/10/28 18:42:45 bernat Exp $

#ifndef BINARY_H
#define BINARY_H

#include "infHeap.h"
#include "addressSpace.h"
#include "codeRange.h"
#include "InstructionSource.h"
#include "ast.h"
#include <map>
#include <functional>
#include <queue>

class fileDescriptor;
class int_function;
class memoryTracker;
class depRelocation;

class BinaryEdit : public AddressSpace {
 public:
    // We must implement the following virtual functions

    // "Read"/"Write" to an address space
    bool readDataSpace(const void *inOther, 
                       u_int amount, 
                       void *inSelf, 
                       bool showError);
    bool readTextSpace(const void *inOther, 
                       u_int amount, 
                       const void *inSelf);
    

    bool writeDataSpace(void *inOther,
                        u_int amount,
                        const void *inSelf);
    bool writeTextSpace(void *inOther,
                        u_int amount,
                        const void *inSelf);

    // Memory allocation
    // We don't specify how it should be done, only that it is. The model is
    // that you ask for an allocation "near" a point, where "near" has an
    // internal, platform-specific definition. The allocation mechanism does its
    // best to give you what you want, but there are no promises - check the
    // address of the returned buffer to be sure.

    Address inferiorMalloc(unsigned size, 
                           inferiorHeapType type=anyHeap,
                           Address near = 0, 
                           bool *err = NULL);
    virtual void inferiorFree(Address item);
    virtual bool inferiorRealloc(Address item, unsigned newSize);

    // Get the pointer size of the app we're modifying
    unsigned getAddressWidth() const;

    /*
    // Until we need these different from AddressSpace,
    // I'm not implementing.
    void *getPtrToInstruction(Address) const;
    bool isValidAddress(const Address &) const;
    */

    // If true is passed for ignore_if_mt_not_set, then an error won't be
    // initiated if we're unable to determine if the program is multi-threaded.
    // We are unable to determine this if the daemon hasn't yet figured out
    // what libraries are linked against the application.  Currently, we
    // identify an application as being multi-threaded if it is linked against
    // a thread library (eg. libpthreads.a on AIX).  There are cases where we
    // are querying whether the app is multi-threaded, but it can't be
    // determined yet but it also isn't necessary to know.
    bool multithread_capable(bool ignore_if_mt_not_set = false);
    
    // Do we have the RT-side multithread functions available
    bool multithread_ready(bool ignore_if_mt_not_set = false);

    // Default to "nope"
    virtual bool hasBeenBound(const relocationEntry &, 
                              int_function *&, 
                              Address) { return false; }

    // Should be easy if the process isn't _executing_ where
    // we're deleting...
    virtual void deleteGeneratedCode(generatedCodeObject *del);

    bool needsPIC();

    BinaryEdit();
    ~BinaryEdit();

    // Same usage pattern as process
    void deleteBinaryEdit();

    // And the "open" factory method.
    static BinaryEdit *openFile(const std::string &file);

    bool writeFile(const std::string &newFileName);
    
    virtual int_function *findOnlyOneFunction(const std::string &name,
                                              const std::string &libname = "",
                                              bool search_rt_lib = true);

    // open a shared library and (optionally) all its dependencies
    bool openSharedLibrary(const std::string &file, bool openDependencies = true);

    // add a shared library relocation
	void addDependentRelocation(Address to, Symbol *referring);

    // search for a shared library relocation
	Address getDependentRelocationAddr(Symbol *referring);

   void setupRTLibrary(BinaryEdit *r);
   BinaryEdit *rtLibrary();
   bool getAllDependencies(std::map<std::string, BinaryEdit* > &deps);

   void markDirty();
   bool isDirty();

   mapped_object *getMappedObject();
   
   int_variable* createTrampGuard();
   void setTrampGuard(int_variable* tg); 

   void setMultiThreadCapable(bool b);

   void addSibling(BinaryEdit *);
   std::vector<BinaryEdit *> &getSiblings();

   bool replaceTrapHandler();
   bool usedATrap();
   bool isMultiThreadCapable();
   std::pair<std::string, BinaryEdit*> openResolvedLibraryName(std::string filename);
 private:
    Address highWaterMark_;
    Address lowWaterMark_;
    bool isDirty_;

    static bool getStatFileDescriptor(const std::string &file,
                                      fileDescriptor &desc);

    bool inferiorMallocStatic(unsigned size);

    bool createMemoryBackingStore(mapped_object *obj);

    bool initialize();

    void makeInitAndFiniIfNeeded();
    
    codeRangeTree* memoryTracker_;

    mapped_object * addSharedObject(const std::string *fullPath);

    std::vector<depRelocation *> dependentRelocations;

    void buildDyninstSymbols(pdvector<Symbol *> &newSyms, 
                             Region *newSec,
                             Module *mod);
    mapped_object *mobj;
    BinaryEdit *rtlib;
    std::vector<BinaryEdit *> siblings;
    bool multithread_capable_;
};

class depRelocation {
	public:
		depRelocation(Address a, Symbol *r) : to(a), referring(r) { }
		Address getAddress() const { return to; }
		Symbol *getReferring() const { return referring; }

	private:
		Address to;
		Symbol *referring;
};

class memoryTracker : public codeRange {
 public:
    memoryTracker(Address a, unsigned s) :
        alloced(false),  dirty(false), a_(a), s_(s) {
        b_ = malloc(s_);
    }

    memoryTracker(Address a, unsigned s, void *b) :
    alloced(false), dirty(false), a_(a), s_(s)
        {
            b_ = malloc(s_);
            memcpy(b_, b, s_);
        }
    ~memoryTracker() { free(b_); }

    Address get_address() const { return a_; }
    unsigned get_size() const { return s_; }
    void *get_local_ptr() const { return b_; }
    void realloc(unsigned newsize) {
      b_ = ::realloc(b_, newsize);
      s_ = newsize;
      assert(b_);
    }

    bool alloced;
    bool dirty;

 private:
    Address a_;
    unsigned s_;
    void *b_;
    
};

#endif // BINARY_H
