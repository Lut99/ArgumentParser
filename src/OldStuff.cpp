    /* The Argument class, which is a baseclass for all three argument types. */
    class Argument {
    protected:
        /* The name by which the Argument is identified, both programatically as in the CLI. */
        std::string name;
        /* An optional description for this argument used in the auto-generated help. */
        std::string description;
        /* An optional category for this argument used in the auto-generated help. */
        std::string category;

        /* Map of Arguments that this Argument has a relation to. */
        std::unordered_map<ArgumentRelation, std::vector<Argument*>> relations;

        /* Constructor for the Argument class which takes a uid. */
        Argument(std::string name) :
            name(name),
            description(""),
            category("Miscellaneous"),
            relations({
                {ArgumentRelation::include, std::vector<Argument*>()},
                {ArgumentRelation::exclude, std::vector<Argument*>()},
                {ArgumentRelation::require, std::vector<Argument*>()},
            })
        {}

        /* Mark the ArgumentParser as friend. */
        friend class ArgumentParser;

    public:
        /* Set the descructor of Argument to virtual. */
        virtual ~Argument() = default;

        /* Sets the description of the Argument. Returns a reference to this argument to allow chaining. */
        virtual Argument& set_description(std::string description) {
            this->description = description;
            return *this;
        }
        /* Sets the category of the Argument. Returns a reference to this argument to allow chaining. */
        virtual Argument& set_category(std::string category) {
            this->category = category;
            return *this;
        }

        /* Returns the name by which the Argument is identified, both programatically as in the CLI. */
        inline std::string get_name() const { return this->name; }
        /* Returns the description of the Argument. */
        inline std::string get_description() const { return this->description; }
        /* Returns the category of the Argument. */
        inline std::string get_category() const { return this->category; }
        /* Returns the type of the Argument, as string. */
        virtual std::string get_arg_type() const = 0;

        /* Copies this Argument, but lets derived classes implement it. */
        virtual Argument* copy() const = 0;

    };

    /* The Positional class: an argument with only a value, not a label. */
    class Positional: public Argument {
    private:
        /* The C++-defined hash-code of the return type. */
        size_t type_hash;
        /* The name of the type associated with this Positional. */
        std::string type_name;
        /* The number of elements for this Positional. */
        size_t n_values;
        /* Pointer to the parser that will be used to parse this type. */
        std::any (*type_parser)(std::vector<std::string>);

        /* Determines the index of this positional. */
        size_t index;
        /* Determines if the positional is optional or not. */
        bool optional;
        /* Determines if the Positional can occur more than once. */
        bool any_number;
        /* Determines if this Positional has a default value in case it is omitted. */
        bool has_default;
        /* The default value of the Positional in case it has one. */
        std::any default_value;

        /* Constructor for the Positional class. */
        Positional(size_t type_hash, std::string type_name, size_t n_values, std::any (*type_parser)(std::vector<std::string>), std::string name, size_t index) :
            Argument(name),
            type_hash(type_hash),
            type_name(type_name),
            n_values(n_values),
            type_parser(type_parser),
            index(index),
            optional(false),
            any_number(false),
            has_default(false),
            default_value(std::any(nullptr))
        {}

        /* Mark the ArgumentParser as friend. */
        friend class ArgumentParser;

    public:
        /* Copy constructor for the Positional class. */
        Positional(const Positional& other) :
            Argument(other),
            type_hash(other.type_hash),
            type_name(other.type_name),
            n_values(other.n_values),
            type_parser(other.type_parser),
            index(other.index),
            optional(other.optional),
            any_number(other.any_number),
            has_default(other.has_default),
            default_value(other.default_value)
        {}
        /* Move constructor for the Positional class. */
        Positional(Positional&& other) :
            Argument(other),
            type_hash(other.type_hash),
            type_name(other.type_name),
            n_values(other.n_values),
            type_parser(other.type_parser),
            index(other.index),
            optional(other.optional),
            any_number(other.any_number),
            has_default(other.has_default),
            default_value(std::move(other.default_value))
        {}
        
        /* Override set_description to return a Positional reference instead. */
        virtual Positional& set_description(std::string description) {
            return (Positional&) Argument::set_description(description);
        }
        /* Override set_category to return a Positional reference instead. */
        virtual Positional& set_category(std::string category) {
            return (Positional&) Argument::set_category(category);
        }
        /* Set whether or not this Positional can occur more than once. If set, then the result will be wrapped in a vector. Note that, if Optional is set to false, the argument still has to occur at least once. */
        Positional& set_any_number(bool any_number) {
            this->any_number = any_number;
            return *this;
        }
        /* Set whether or not this Positional is optional. Returns a reference to allow chaining. */
        Positional& set_optional(bool optional) {
            this->optional = optional;
            return *this;
        }
        /* Set the default input value of this Positional, in case it's only a single value. Here, it allows the user to omit the Positional, but the argument still appears in the Arguments class as if it was given a value. Returns a reference to allow chaining. */
        Positional& set_default(std::any default_value) {
            this->has_default = true;
            this->default_value = default_value;
            return *this;
        }
        /* Clears the default value from this Positional. Returns a reference to allow chaining. */
        Positional& clear_default() {
            this->has_default = false;
            return *this;
        }

        /* Returns the name of this Positional's type. */
        inline std::string get_type_name() const { return this->type_name; }
        /* Returns the hash of this Positional's type. */
        inline size_t get_type_hash() const { return this->type_hash; }
        /* Returns the index of this Positional. */
        inline size_t get_index() const { return this->index; }
        /* Returns whether this Positional is optional or not. */
        inline bool get_optional() const { return this->optional; }
        /* Returns whether this Positional can be given more than once or not. */
        inline bool get_any_number() const { return this->any_number; }
        /* Returns whether this Positional has a default value defined. */
        inline bool has_default_value() const { return this->has_default; }
        /* Returns the default value for this Postitional. Note that this makes no sense if has_default = false. */
        inline std::any get_default_value() const { return this->default_value; }
        /* Returns the type of the Argument, as string. */
        virtual std::string get_arg_type() const { return "positional"; };

        /* Copy assignment operator for the Positional class. */
        Positional& operator=(const Positional& other) = delete;
        /* Move assignment operator for the Positional class. */
        Positional& operator=(Positional&& other) = delete;

        /* Copies this Argument, but lets derived classes implement it. */
        virtual Positional* copy() const { return new Positional(*this); }

    };

    /* The Flag class: an argument with only a label, not a value. */
    class Flag: public Argument {
    protected:
        /* The shortlabel (single-character ID) of the Flag. */
        char shortlabel;

        /* Constructor for the Flag class. */
        Flag(std::string name, char shortlabel) :
            Argument(name),
            shortlabel(shortlabel)
        {}

        /* Mark the ArgumentParser as friend. */
        friend class ArgumentParser;

    public:
        /* Copy constructor for the Flag class. */
        Flag(const Flag& other) :
            Argument(other),
            shortlabel(other.shortlabel)
        {}
        /* Move constructor for the Flag class. */
        Flag(Flag&& other) :
            Argument(other),
            shortlabel(other.shortlabel)
        {}

        /* Override set_description to return a Flag reference instead. */
        virtual Flag& set_description(std::string description) {
            return (Flag&) Argument::set_description(description);
        }
        /* Override set_category to return a Flag reference instead. */
        virtual Flag& set_category(std::string category) {
            return (Flag&) Argument::set_category(category);
        }

        /* Returns the shortlabel (single-character ID) of the Flag. */
        inline char get_shortlabel() const { return this->shortlabel; }
        /* Returns the type of the Argument, as string. */
        virtual std::string get_arg_type() const { return "flag"; };

        /* Copy assignment operator for the Flag class. */
        Flag& operator=(const Flag& other) = delete;
        /* Move assignment operator for the Flag class. */
        Flag& operator=(Flag&& other) = delete;

        /* Copies this Argument, but lets derived classes implement it. */
        virtual Flag* copy() const { return new Flag(*this); }

    };

    /* The Option class: an argument with a value and a label. Inherits from a Flag, as it's basically a flag with value. */
    class Option: public Flag {
    private:
        /* The C++-defined hash-code of the return type. */
        size_t type_hash;
        /* The name of the type associated with this Positional. */
        std::string type_name;
        /* The number of elements for this Positional. */
        size_t n_values;
        /* Pointer to the parser that will be used to parse this type. */
        std::any (*type_parser)(std::vector<std::string>);

        /* A placeholder string for what should be shown at the type. Is the type name by default. */
        std::string placeholder;

        /* Determines if the Option is optional or not. */
        bool optional;
        /* Determines if the Positional can occur more than once. */
        bool any_number;
        /* Determines if this Option has a default value in case it is omitted. */
        bool has_default;
        /* The default value of the Option in case it has one. */
        std::any default_value;

        /* Constructor for the Option class. */
        Option(size_t type_hash, std::string type_name, size_t n_values, std::any (*type_parser)(std::vector<std::string>), std::string name, char shortlabel) :
            Flag(name, shortlabel),
            type_hash(type_hash),
            type_name(type_name),
            n_values(n_values),
            type_parser(type_parser),
            placeholder(upperify(type_name)),
            optional(false),
            any_number(false),
            has_default(false),
            default_value(std::any(nullptr))
        {}

        /* Mark the ArgumentParser as friend. */
        friend class ArgumentParser;

    public:
        /* Copy constructor for the Option class. */
        Option(const Option& other) :
            Flag(other),
            type_hash(other.type_hash),
            type_name(other.type_name),
            n_values(other.n_values),
            type_parser(other.type_parser),
            placeholder(other.placeholder),
            optional(other.optional),
            any_number(other.any_number),
            has_default(other.has_default),
            default_value(other.default_value)
        {}
        /* Move constructor for the Option class. */
        Option(Option&& other) :
            Flag(other),
            type_hash(other.type_hash),
            type_name(other.type_name),
            n_values(other.n_values),
            type_parser(other.type_parser),
            placeholder(other.placeholder),
            optional(other.optional),
            any_number(other.any_number),
            has_default(other.has_default),
            default_value(std::move(other.default_value))
        {}

        /* Override set_description to return an Option reference instead. */
        virtual Option& set_description(std::string description) {
            return (Option&) Argument::set_description(description);
        }
        /* Override set_category to return an Option reference instead. */
        virtual Option& set_category(std::string category) {
            return (Option&) Argument::set_category(category);
        }
        /* Set a custom placeholder for the value of the Option. Is the name of the type by default. */
        Option& set_placeholder(std::string placeholder) {
            this->placeholder = upperify(placeholder);
            return *this;
        }
        /* Set whether or not this Option is optional. Returns a reference to allow chaining. */
        Option& set_optional(bool optional) {
            this->optional = optional;
            return *this;
        }
        /* Set whether or not this Option can occur more than once. If set, then the result will be wrapped in a vector. Note that, if Optional is set to false, the argument still has to occur at least once. */
        Option& set_any_number(bool any_number) {
            this->any_number = any_number;
            return *this;
        }
        /* Set the default input value of this Option, in case it's only a single value. Here, it means that the Option can be given but no value has to be specified. If the Option is mandatory, then giving it a default value also causes it to appear in the resulting Arguments dictionary. Returns a reference to allow chaining. */
        Option& set_default(std::any default_value) {
            this->has_default = true;
            this->default_value = default_value;
            return *this;
        }
        /* Clears the default value from this Option. Returns a reference to allow chaining. */
        Option& clear_default() {
            this->has_default = false;
            return *this;
        }

        /* Returns the name of this Positional's type. */
        inline std::string get_type_name() const { return this->type_name; }
        /* Returns the hash of this Positional's type. */
        inline size_t get_type_hash() const { return this->type_hash; }
        /* Returns the shortlabel of the Option. */
        inline char get_shortlabel() const { return this->shortlabel; }
        /* Returns the placeholder of this Option. */
        inline std::string get_placeholder() const { return this->placeholder; }
        /* Returns whether this Option is optional or not. */
        inline bool get_optional() const { return this->optional; }
        /* Returns whether this Positional can be given more than once or not. */
        inline bool get_any_number() const { return this->any_number; }
        /* Returns whether this Positional has a default value defined. */
        inline bool has_default_value() const { return this->has_default; }
        /* Returns the default value for this Option. Makes no sense if has_default == false. */
        inline std::any get_default_value() const { return this->default_value; }
        /* Returns the type of the Argument, as string. */
        virtual std::string get_arg_type() const { return "option"; };

        /* Copy assignment operator for the Option class. */
        Option& operator=(const Option& other) = delete;
        /* Move assignment operator for the Option class. */
        Option& operator=(Option&& other) = delete;

        /* Copies this Argument, but lets derived classes implement it. */
        virtual Option* copy() const { return new Option(*this); }

    };

    /* The MultiArgument class: forms the baseclass for all arguments which can have children or nested arguments. */
    class MultiArgument : public Argument {
    protected:
        /* A list of arguments who's management will fall under this MultiArgument. */
        std::vector<Argument*> args;
        /* The number of elements that will be allocated at a time (and pre-allocated) whenever new memory is needed, to reduce memory allocation times. */
        size_t reserve_chunk;

        /* Constructor for the MultiArgument class, which takes the type of the MultiArgument and its initial size. */
        MultiArgument(ArgumentType arg_type, size_t reserve_chunk = 5) :
            Argument(arg_type),
            reserve_chunk(reserve_chunk)
        {
            this->args.reserve(reserve_chunk);
        }

    public:
        /* Mutable iterator for the MultiArgument class. Returns a reference to the internal Arguments, in order. */
        class iterator {
        protected:
            /* List of the AtomicArguments found in the nested MultiArgument. */
            std::vector<AtomicArgument*> args;
            /* Index in the list of arguments. */
            size_t pos;

            /* Constructor which directly takes an args list, mainly for almost-copy purposes. */
            iterator(const std::vector<AtomicArgument*>& new_args, size_t pos = 0) :
                args(new_args),
                pos(pos)
            {}

        public:
            /* Constructor for the iterator class which takes a pointer to the MultiArgument and optionally the initial position. While the iterator copies all arguments and so the MultiArgument class can be freely edited while iterating, it does assume none of the Arguments it stores will be deallocated. */
            iterator(MultiArgument* arg, size_t pos = 0) :
                pos(pos)
            {
                std::vector<Argument*> todo = std::vector<Argument*>(arg->args.rbegin(), arg->args.rend());
                while (todo.size() > 0) {
                    // Get athe first element from the stack
                    Argument* arg = todo.at(todo.size() - 1);
                    todo.pop_back();

                    if (arg->is_atomic()) {
                        // It's an AtomicArgument, so return that
                        this->args.push_back((AtomicArgument*) arg);
                    } else {
                        // It's a MultiArgument, so push all its elements to the start of the queue (for depth-first purposes)
                        MultiArgument* m_arg = (MultiArgument*) arg;
                        todo.insert(todo.end(), m_arg->args.rbegin(), m_arg->args.rend());
                    }
                }
            }
            /* Virtual destructor for the iterator class. */
            virtual ~iterator() = default;


            /* Returns true if this iterator does equal the given one. */
            inline bool operator==(const iterator& other) { return this->args == other.args && this->pos == other.pos; }
            /* Returns true if this iterator does NOT equal to given one. */
            inline bool operator!=(const iterator& other) { return this->args != other.args || this->pos != other.pos; }
            /* Returns true if this iterator points to the same MultiArgument as the given iterator, but this iterator is less advanced. */
            inline bool operator<(const iterator& other) { return this->args == other.args && this->pos < other.pos; }
            /* Returns true if this iterator points to the same MultiArgument as the given iterator, but this iterator is less or equally advanced. */
            inline bool operator<=(const iterator& other) { return this->args == other.args && this->pos <= other.pos; }
            /* Returns true if this iterator points to the same MultiArgument as the given iterator, but this iterator is more advanced. */
            inline bool operator>(const iterator& other) { return this->args == other.args && this->pos > other.pos; }
            /* Returns true if this iterator points to the same MultiArgument as the given iterator, but this iterator is more or equally advanced. */
            inline bool operator>=(const iterator& other) { return this->args == other.args && this->pos >= other.pos; }

            /* Moves the iterator forward by one position, but keeps it bounded so it never exceedes MultiArgument::end(). */
            virtual iterator& operator++() {
                ++this->pos;
                if (this->pos > this->args.size()) { this->pos = this->args.size(); }
                return *this;
            }
            /* Retruns a new iterator which is forwarded by n positions compared to this one. Makes sure it never exceedes MultiArgument::end(). */
            iterator operator+(size_t n) const {
                size_t new_pos = this->pos + n;
                if (new_pos > this->args.size()) { new_pos = this->args.size(); }
                return iterator(this->args, new_pos);
            }
            /* Moves the iterator forward by n positions, but keeps it bounded so it never exceedes MultiArgument::end(). */
            virtual iterator& operator+=(size_t n) {
                this->pos += n;
                if (this->pos > this->args.size()) { this->pos = this->args.size(); }
                return *this;
            }
            /* Allows random access to the iterator, by returning an iterator which is set at the given index. Never exceeds MultiArgument::end(), even if n is larger than it. */
            iterator operator[](size_t index) const {
                if (index > this->args.size()) { index = this->args.size(); }
                return iterator(this->args, index);
            }

            /* Dereferences the iterator. */
            inline AtomicArgument& operator*() { return *(this->args.at(this->pos)); }

            /* Swap operator for the iterator class. */
            friend void swap(MultiArgument::iterator& i1, MultiArgument::iterator& i2);

        };
        /* Non-mutable iterator for the MultiArgument class. Returns a constant reference to the internal Arguments, in order. */
        class const_iterator : public iterator {
        private:
            /* Constructor for the const_iterator class which takes a poitner to an args list. */
            const_iterator(const std::vector<AtomicArgument*>& args, size_t pos = 0) :
                iterator(args, pos)
            {}

        public:
            /* Constructor for the const_iterator class which takes a pointer to the (const) MultiArgument and optionally the initial position. Note that all arguments are copied, so they cannot be altered. */
            const_iterator(const MultiArgument* arg, size_t pos = 0) :
                iterator(const_cast<MultiArgument*>(arg), pos)
            {
                // Be sure to copy the pointers to the arg list
                for (size_t i = 0; i < this->args.size(); i++) {
                    this->args.at(i) = this->args.at(i)->copy();
                }
            }
            /* Copy constructor for the const_iterator class. */
            const_iterator(const const_iterator& other) :
                iterator(other)
            {
                // Be sure to copy the pointers to the arg list
                for (size_t i = 0; i < this->args.size(); i++) {
                    this->args.at(i) = other.args.at(i)->copy();
                }
            }
            /* Move constructor for the const_iterator class. */
            const_iterator(const_iterator&& other) :
                iterator(other)
            {
                // Replace all items with nullptr in other.args
                for (size_t i = 0; i < this->args.size(); i++) {
                    other.args.at(i) = nullptr;
                }
            }
            /* Destructor for the const_iterator class. */
            virtual ~const_iterator() {
                for (size_t i = 0; i < this->args.size(); i++) {
                    // Only deallocate if not null
                    AtomicArgument* arg = this->args.at(i);
                    if (arg != nullptr) { delete arg; }
                }
            }

            /* Moves the const_iterator forward by one position, but keeps it bounded so it never exceedes MultiArgument::end(). */
            virtual const_iterator& operator++() { return (const_iterator&) iterator::operator++(); }
            /* Retruns a new const_iterator which is forwarded by n positions compared to this one. Makes sure it never exceedes MultiArgument::end(). */
            const_iterator operator+(size_t n) const {
                size_t new_pos = this->pos + n;
                if (new_pos > this->args.size()) { new_pos = this->args.size(); }
                return const_iterator(this->args, new_pos);
            }
            /* Moves the const_iterator forward by n positions, but keeps it bounded so it never exceedes MultiArgument::end(). */
            virtual const_iterator& operator+=(size_t n) { return (const_iterator&) iterator::operator+=(n); }
            /* Allows random access to the const_iterator, by returning a const_iterator which is set at the given index. Never exceeds MultiArgument::end(), even if n is larger than it. */
            const_iterator operator[](size_t index) const {
                if (index > this->args.size()) { index = this->args.size(); }
                return const_iterator(this->args, index);
            }

            /* Dereferences the const_iterator. Note that the returned Arguments are actually copies of the arguments in the MultiArgument. */
            inline const AtomicArgument& operator*() const { return *(this->args.at(this->pos)); }

            /* Copy assignment operator for the const_iterator class. */
            inline const_iterator& operator=(const const_iterator& other) { return *this = const_iterator(other); }
            /* Move assignment operator for the const_iterator class. */
            const_iterator& operator=(const_iterator&& other) {
                if (this != &other) { swap(*this, other); }
                return *this;
            }
            /* Swap operator for the const_iterator class. */
            friend void swap(MultiArgument::const_iterator& ci1, MultiArgument::const_iterator& ci2);

        };

        /* Copy constructor for the MultiArgument class. */
        MultiArgument(const MultiArgument& other) :
            Argument(other),
            reserve_chunk(other.reserve_chunk)
        {
            this->args.reserve(other.args.capacity());
            for (Argument* arg : other.args) {
                this->args.push_back(arg->copy());
            }
        }
        /* Move constructor for the MultiArgument class. */
        MultiArgument(MultiArgument&& other) :
            Argument(other),
            reserve_chunk(other.reserve_chunk)
        {
            this->args.reserve(other.args.capacity());
            for (size_t i = 0; i < other.args.size(); i++) {
                this->args.push_back(other.args.at(i));
                other.args.at(i) = nullptr;
            }
        }
        /* Destructor for the MultiArgument class. */
        virtual ~MultiArgument() {
            for (Argument* arg : this->args) {
                // Only delete if not-nullptr, to accomodate being robbed
                if (arg != nullptr) { delete arg; }
            }
        }

        /* Adds a new Positional of given type to the MultiArgument. Might throw errors if the addition is illegal in some way. Returns a reference to the Positional so that it can be configured in detail. */
        template <class T, typename=std::enable_if_t<std::is_base_of<BaseType, T>::value> >
        Positional& add_positional(const std::string& name) {
            const std::string context = "MultiArgument::add_positional()";

            // First, check if n_values is larger than 0
            if (T::n_values == 0) { throw IllegalNValuesException(context, T::n_values); }

            // Then, check the validity of the name
            char valid = is_valid_name(name);
            if (valid != '\0') { throw IllegalNameException(context, name, valid); }
            // Check if it is unique
            if (this->has_name(name)) { throw DuplicateNameException(context, name); }

            // So far, it checks out, so add it
            Positional* pos = new Positional(name, T::runtime());
            if (this->args.size() == this->args.capacity()) { this->args.reserve(this->args.size() + this->reserve_chunk); }
            this->args.push_back(pos);

            // Return a reference
            return *pos;
        }
        /* Adds a new Option of given type to the MultiArgument. Might throw errors if the addition is illegal in some way. Returns a reference to the Option so that it can be configured in detail. */
        template <class T, typename=std::enable_if_t<std::is_base_of<BaseType, T>::value> >
        Option& add_option(char shortlabel, const std::string& name) {
            const std::string context = "MultiArgument::add_option()";

            // First, check if n_values is larger than 0
            if (T::n_values == 0) { throw IllegalNValuesException(context, T::n_values); }

            // Then, check the validity of the name & the shortlabel
            char valid = is_valid_name(name);
            if (valid != '\0') { throw IllegalNameException(context, name, valid); }
            if (!is_valid_shortlabel(shortlabel)) { throw IllegalShortlabelException(context, shortlabel); }

            // Check if both the name and the shortlabel are unique
            if (this->has_name(name)) { throw DuplicateNameException(context, name); }
            else if (this->has_shortlabel(shortlabel)) { throw DuplicateShortlabelException(context, shortlabel); }

            // So far, it checks out, so add it
            Option* opt = new Option(name, shortlabel, T::runtime());
            if (this->args.size() == this->args.capacity()) { this->args.reserve(this->args.size() + this->reserve_chunk); }
            this->args.push_back(opt);

            // Return a reference
            return *opt;
        }
        /* Adds a new Flag to the MultiArgument. Might throw errors if the addition is illegal in some way. Returns a reference to the Flag so that it can be configured in detail. */
        Flag& add_flag(char shortlabel, const std::string& name) {
            const std::string context = "MultiArgument::add_flag()";

            // Then, check the validity of the name & the shortlabel
            char valid = is_valid_name(name);
            if (valid != '\0') { throw IllegalNameException(context, name, valid); }
            if (!is_valid_shortlabel(shortlabel)) { throw IllegalShortlabelException(context, shortlabel); }

            // Check if both the name and the shortlabel are unique
            if (this->has_name(name)) { throw DuplicateNameException(context, name); }
            else if (this->has_shortlabel(shortlabel)) { throw DuplicateShortlabelException(context, shortlabel); }

            // So far, it checks out, so add it
            Flag* flg = new Flag(name, shortlabel);
            if (this->args.size() == this->args.capacity()) { this->args.reserve(this->args.size() + this->reserve_chunk); }
            this->args.push_back(flg);

            // Return a reference
            return *flg;
        }
        /* Adds a new IncludedGroup to the MultiArgument. The name given is in a different namespace for MultiArguments only, and will throw a DuplicateNameException is a MultiArgument with that name is already found. Returns a reference to the new group so that Arguments can be added and it can be configured in detail. */
        IncludedGroup& add_included_group(const std::string& name) {
            // Create it, then return it
            IncludedGroup* inc = new IncludedGroup(this->reserve_chunk);
            if (this->args.size() == this->args.capacity()) { this->args.reserve(this->args.size() + this->reserve_chunk); }
            this->args.push_back(inc);
            return *inc;
        }
        /* Adds a new ExcludedGroup to the MultiArgument. Returns a reference to the new group so that Arguments can be added and it can be configured in detail. */
        ExcludedGroup& add_excluded_group(const std::string& name) {
            // Create it, then return it
            ExcludedGroup* exc = new ExcludedGroup(this->reserve_chunk);
            if (this->args.size() == this->args.capacity()) { this->args.reserve(this->args.size() + this->reserve_chunk); }
            this->args.push_back(exc);
            return *exc;
        }
        /* Adds a new RequiredGroup to the MultiArgument. Returns a reference to the new group so that Arguments can be added and it can be configured in detail. */
        RequiredGroup& add_required_group(const std::string& name) {
            // Create it, then return it
            RequiredGroup* req = new RequiredGroup(this->reserve_chunk);
            if (this->args.size() == this->args.capacity()) { this->args.reserve(this->args.size() + this->reserve_chunk); }
            this->args.push_back(req);
            return *req;
        }

        /* Validates given Arguments dictionary containing parsed Arguments according to the internal set of rules, dependent on the MultiArgument type. */
        virtual bool validate(const Arguments& args) const {
            // For the MultiArgument itself, always return true as there are no restrictions. */
            return true;
        }

        /* Returns a mutable reference to an Argument based on its index in this MultiArgument (shallow search). */
        Argument* at(size_t index) {
            return const_cast<Argument*>(((const MultiArgument*) this)->at(index));
        }
        /* Returns a non-mutable reference to an Argument based on its index in this MultiArgument (shallow search). */
        const Argument* at(size_t index) const {
            if (index >= this->args.size()) {
                throw runtime_error("Index " + std::to_string(index) + " is too large for MultiArgument with size " + std::to_string(this->atomic_size()) + ".");
            }

            return this->args.at(index);
        }

        /* Returns a (mutable) iterator pointing to the AtomicArgument with the given name. If there is no such argument, the iterator points to MultiArgument::end() instead. */
        iterator find(const std::string& name) {
            size_t i = 0;
            for (AtomicArgument& arg : *this) {
                if (arg.get_name() == name) { return iterator(this, i); }
                ++i;
            }
            return this->end();
        }
        /* Returns a (non-mutable) iterator pointing to the AtomicArgument with the given name. If there is no such argument, the iterator points to MultiArgument::end() instead. */
        const_iterator find(const std::string& name) const {
            size_t i = 0;
            for (const AtomicArgument& arg : *this) {
                if (arg.get_name() == name) { return const_iterator(this, i); }
                ++i;
            }
            return this->end();
        }

        /* Returns the number of Arguments stored in this MultiArgument (shallow, including nested MultiArguments). */
        inline size_t size() const { return this->args.size(); }
        /* Returns the number of AtomicArgument-derived Arguments stored in this MultiArgument (deep, only AtomicArguments). */
        size_t atomic_size() const {
            size_t result = 0;
            for (Argument* arg : this->args) {
                // If it's atomic, increment by one; otherwise, increment by the atomic_size() of that argument.
                if (arg->is_atomic()) {
                    ++result;
                } else {
                    result += ((MultiArgument*) arg)->atomic_size();
                }
            }
            return result;
        }

        /* Returns true if this MultiArgument contains an AtomicArgument with given name. */
        virtual bool has_name(const std::string& name) const {
            for (Argument* arg : this->args) {
                if (arg->has_name(name)) { return true; }
            }
            return false;
        }
        /* Returns true if this MultiArgument contains an AtomicArgument with given shortlabel. */
        virtual bool has_shortlabel(char shortlabel) const {
            for (Argument* arg : this->args) {
                if (arg->has_shortlabel(shortlabel)) { return true; }
            }
            return false;
        }

        /* Returns a mutable, deep iterator to the beginning of the MultiArgument. */
        inline iterator begin() { return iterator(this); }
        /* Returns a non-mutable, deep iterator to the beginning of the MultiArgument. */
        inline const_iterator begin() const { return const_iterator(this); }
        /* Returns a non-mutable, deep iterator to the beginning of the MultiArgument. */
        inline const_iterator cbegin() const { return ((const MultiArgument*) this)->begin(); }
        /* Returns a mutable, deep iterator to the end of the MultiArgument. */
        inline iterator end() { return iterator(this, this->args.size()); }
        /* Returns a non-mutable, deep iterator to the end of the MultiArgument. */
        inline const_iterator end() const { return const_iterator(this, this->args.size()); }
        /* Returns a non-mutable, deep iterator to the end of the MultiArgument. */
        inline const_iterator cend() const { return ((const MultiArgument*) this)->end(); }
        
        /* Copy assignment operator for the MultiArgument class. */
        inline MultiArgument& operator=(const MultiArgument& other) { return *this = MultiArgument(other); }
        /* Move assignment operator for the MultiArgument class. */
        MultiArgument& operator=(MultiArgument&& other) {
            if (this != &other) { swap(*this, other); }
            return *this;
        }
        /* Swap operator for the MultiArgument class. */
        friend void swap(MultiArgument& ma1, MultiArgument& ma2);

        /* Allows derived classes of the MultiArgument to copy themselves. */
        virtual MultiArgument* copy() const { return new MultiArgument(*this); };

    };
    /* Swap operator for the MultiArgument class. */
    void swap(MultiArgument& ma1, MultiArgument& ma2) {
        using std::swap;

        swap((Argument&) ma1, (Argument&) ma2);
        swap(ma1.args, ma2.args);
        swap(ma1.reserve_chunk, ma2.reserve_chunk);
    }
    /* Swap operator for the iterator class. */
    void swap(MultiArgument::iterator& i1, MultiArgument::iterator& i2) {
        using std::swap;

        swap(i1.args, i2.args);
        swap(i1.pos, i2.pos);
    }
    /* Swap operator for the const_iterator class. */
    void swap(MultiArgument::const_iterator& ci1, MultiArgument::const_iterator& ci2) {
        using std::swap;

        swap((MultiArgument::iterator&) ci1, (MultiArgument::iterator&) ci2);
    }



    /* Exception for when a Positional was not optional after an optional Positional has been declared. */
    class OptionalPositionalException : public ProgrammingException {
    private:
        /* The name of the Positional that was illegal. */
        std::string name;

    public:
        /* Constructor for the OptionalPositionalException which takes the uid of the illegal Positional. */
        OptionalPositionalException(const std::string& context, const std::string& name) :
            ProgrammingException(context, generate_message(context, "Encountered mandatory Positional '" + name + "' after optional Positionals are declared.")),
            name(name)
        {}

        /* Returns the uid of the Positional that was causing this exception. */
        inline std::string get_name() const { return this->name; }

        /* Allows derived classes to copy themselves. */
        virtual OptionalPositionalException* copy() const {
            return new OptionalPositionalException(*this);
        }

    };
    /* Exception for when a Positional is set to be repeated any number of times, but is not the last Positional. */
    class AnyNumberPositionalException : public ProgrammingException {
    private:
        /* The name of the Positional that was illegal. */
        std::string name;

    public:
        /* Constructor for the AnyNumberPositionalException which takes the uid of the illegal Positional. */
        AnyNumberPositionalException(const std::string& context, const std::string& name) :
            ProgrammingException(context, generate_message(context, "Encountered non-last Positional '" + name + "' that can be repeated any number of times, which is ambigious.")),
            name(name)
        {}

        /* Returns the uid of the Positional that was causing this exception. */
        inline std::string get_name() const { return this->name; }

        /* Allows derived classes to copy themselves. */
        virtual AnyNumberPositionalException* copy() const {
            return new AnyNumberPositionalException(*this);
        }

    };

    /* Baseclass exception for when the same things are expected, but different things are given. */
    class DifferentException : public ProgrammingException {
    public:
        /* Constructor for the DifferentException which takes a context and optionally a message. */
        DifferentException(const std::string& context, const std::string& message = "") :
            ProgrammingException(context, message)
        {}

        /* Allows derived classes to copy themselves. */
        virtual DifferentException* copy() const {
            return new DifferentException(*this);
        }

    };
    /* Exception for when two arguments are given that are expected to be of the same type, but are actually of a different type. */
    class DifferentArgTypeException : public DifferentException {
    private:
        /* The type of the first argument given. */
        std::string type1;
        /* The type of the second argument given. */
        std::string type2;

    public:
        /* Constructor for the DifferentArgTypeException, which takes a context, the type of the first argument (as string) and the type of the second argument (as string). */
        DifferentArgTypeException(const std::string& context, const std::string& first_type, const std::string& second_type) :
            DifferentException(context, generate_message(context, "Expected arguments to have the same type, but instead got " + first_type + " and " + second_type + ".")),
            type1(first_type),
            type2(second_type)
        {}

        /* Returns the type of the first argument given (as string). */
        inline std::string get_first_type() const { return this->type1; }
        /* Returns the type of the second argument given (as string). */
        inline std::string get_second_type() const { return this->type2; }

        /* Allows derived classes to copy themselves. */
        virtual DifferentArgTypeException* copy() const {
            return new DifferentArgTypeException(*this);
        }
        
    };

    /* Baseclass exception for when different things are expected, but the same ones are given. */
    class SameException : public ProgrammingException {
    public:
        /* Constructor for the SameException which takes a context and optionally a message. */
        SameException(const std::string& context, const std::string& message = "") :
            ProgrammingException(context, message)
        {}

        /* Allows derived classes to copy themselves. */
        virtual SameException* copy() const {
            return new SameException(*this);
        }

    };
    /* Exception for when two arguments are given that are actually the same argument. */
    class SameArgumentException : public ProgrammingException {
    private:
        /* The name of the argument that occured more than once. */
        std::string name;

    public:
        /* Constructor for the SameArgumentException, which takes a context and the name of the Argument that was illegally given multiple times. */
        SameArgumentException(const std::string& context, const std::string& name) :
            ProgrammingException(context, generate_message(context, "Expected different arguments, but instead got argument '" + name + "' more than once.")),
            name(name)
        {}

        /* Returns the name of the argument that illegally occured more than once. */
        inline std::string get_name() const { return this->name; }

        /* Allows derived classes to copy themselves. */
        virtual SameArgumentException* copy() const {
            return new SameArgumentException(*this);
        }
        
    };

    /* Baseclass exception for when something isn't what we expected it to be. */
    class MismatchException : public ProgrammingException {
    public:
        /* Constructor for the MismatchException that takes a context and an optional message. */
        MismatchException(const std::string& context, const std::string& message = "") :
            ProgrammingException(context, message)
        {}

        /* Allows derived classes to copy themselves. */
        virtual MismatchException* copy() const {
            return new MismatchException(*this);
        }

    };
    /* Exception for when an accessed argument is supposed to be a Position, but it wasn't. */
    class PositionalMismatchException : public MismatchException {
    private:
        /* The name of the incorrectly accessed argument. */
        std::string name;

    public:
        /* Constructor for the PositionalMismatchException which takes a context (the function where it occured), the name of the Positional and the index of the Positional. */
        PositionalMismatchException(const std::string& context, const std::string& name) :
            MismatchException(context, generate_message(context, "Tried to access a non-Positional argument '" + name + "' as if it were a Positional")),
            name(name)
        {}

        /* Returns the name of the mismatched argument. */
        inline std::string get_name() const { return this->name; }

        /* Allows derived classes to copy themselves. */
        virtual PositionalMismatchException* copy() const {
            return new PositionalMismatchException(*this);
        }

    };
    /* Exception for when an accessed argument is supposed to be a Flag, but it wasn't. */
    class FlagMismatchException : public MismatchException {
    private:
        /* The name of the incorrectly accessed argument. */
        std::string name;

    public:
        /* Constructor for the PositionalMismatchException which take a context (the function where it occured) and the name of the mismatched argument. */
        FlagMismatchException(const std::string& context, const std::string& name) :
            MismatchException(context, generate_message(context, "Tried to access a non-Flag argument '" + name + "' as if it were a Flag")),
            name(name)
        {}

        /* Returns the name of the mismatched argument. */
        inline std::string get_name() const { return this->name; }

        /* Allows derived classes to copy themselves. */
        virtual FlagMismatchException* copy() const {
            return new FlagMismatchException(*this);
        }

    };
    /* Exception for when an accessed argument is supposed to be an Option, but it wasn't. */
    class OptionMismatchException : public MismatchException {
    private:
        /* The name of the incorrectly accessed argument. */
        std::string name;

    public:
        /* Constructor for the OptionMismatchException which take a context (the function where it occured) and the name of the mismatched argument. */
        OptionMismatchException(const std::string& context, const std::string& name) :
            MismatchException(context, generate_message(context, "Tried to access a non-Option argument '" + name + "' as if it were an Option")),
            name(name)
        {}

        /* Returns the name of the mismatched argument. */
        inline std::string get_name() const { return this->name; }

        /* Allows derived classes to copy themselves. */
        virtual OptionMismatchException* copy() const {
            return new OptionMismatchException(*this);
        }

    };
    /* Exception for when an accessed argument is supposed to have a value, but it didn't. */
    class ValueMismatchException : public MismatchException {
    private:
        /* Name of the argument which was incorrectly presumed to have a value. */
        std::string name;

    public:
        /* Constructor for the PositionalMismatchException which take a context (the function where it occured) and the name of the non-value carrying argument. */
        ValueMismatchException(const std::string& context, const std::string& name) :
            MismatchException(context, generate_message(context, "Tried to access argument '" + name + "' as if it had a value, but it didn't (it was a Flag).")),
            name(name)
        {}

        /* Returns the name of the argument which was supposed to have a value, but didn't. */
        inline std::string get_name() const { return this->name; }

        /* Allows derived classes to copy themselves. */
        virtual ValueMismatchException* copy() const {
            return new ValueMismatchException(*this);
        }

    };
    /* Exception for when the returned type of a parser does not match the defined type in a DerivedType. */
    class ParserMismatchException : public MismatchException {
    private:
        /* The type name of the DerivedType that went wrong. */
        std::string type_name;

    public:
        /* Constructor for the ParserMismatchException that takes a context and the name of the erroring DerivedType. */
        ParserMismatchException(const std::string& context, const std::string& type_name) :
            MismatchException(context, generate_message(context, "The return type of the parser for Type '" + type_name + "' does not match the Type's type.")),
            type_name(type_name)
        {}

        /* Returns the type name of the DerivedType who's parser and type do not match. */
        inline std::string get_type_name() const { return this->type_name; }

        /* Allows derived classes to copy themselves. */
        virtual ParserMismatchException* copy() const {
            return new ParserMismatchException(*this);
        }

    };
    /* Exception for when a given type is not the one we expect. */
    class TypeMismatchException : public MismatchException {
    private:
        /* The name of the expected type. */
        std::string expected;
        /* The name of the received type. */
        std::string given;

    public:
        TypeMismatchException(const std::string& context, const std::string& expected_type_name, const std::string& given_type_name) :
            MismatchException(context, generate_message(context, "The requested type is not what we expect: expected '" + expected_type_name + "', got '" + given_type_name + "'.")),
            expected(expected_type_name),
            given(given_type_name)
        {}

        /* Returns the expected type's name. */
        inline std::string get_expected() const { return this->expected; }
        /* Returns the given type's name. */
        inline std::string get_given() const { return this->given; }

        /* Allows derived classes to copy themselves. */
        virtual TypeMismatchException* copy() const {
            return new TypeMismatchException(*this);
        }

    };
    /* Exception for when the ArgumentType is not the one we expect. */
    class ArgTypeMismatchException : public MismatchException {
    private:
        /* The name of the Argument in question. */
        std::string name;
        /* The expected type. */
        ArgumentType expected;
        /* The received type. */
        ArgumentType given;

    public:
        ArgTypeMismatchException(const std::string& context, const std::string& name, const ArgumentType expected_type, const ArgumentType given_type) :
            MismatchException(context, generate_message(context, "Argument '" + name + "' is a " + argtype_name_map[(int) expected_type] + ", but is treated as a " + argtype_name_map[(int) given_type] + ".")),
            name(name),
            expected(expected_type),
            given(given_type)
        {}

        /* Returns the name of the Argument who's type was mismatching. */
        inline std::string get_name() const { return this->name; }
        /* Returns the expected type. */
        inline ArgumentType get_expected() const { return this->expected; }
        /* Returns the given type. */
        inline ArgumentType get_given() const { return this->given; }

        /* Allows derived classes to copy themselves. */
        virtual ArgTypeMismatchException* copy() const {
            return new ArgTypeMismatchException(*this);
        }

    };
    /* Exception for when a given argument can be repeated whereas we expect it not to. */
    class AnyNumberMismatchException : public MismatchException {
    private:
        /* The name of the argument that shouldn't be able to repeat. */
        std::string name;

    public:
        AnyNumberMismatchException(const std::string& context, const std::string& name) :
            MismatchException(context, generate_message(context, "Argument '" + name + "' can be specified multiple times, and must be retrieved using Arguments::getp().")),
            name(name)
        {}

        /* Returns the name of the argument that could be specified multiple times against our expectations. */
        inline std::string get_name() const { return this->name; }

        /* Allows derived classes to copy themselves. */
        virtual AnyNumberMismatchException* copy() const {
            return new AnyNumberMismatchException(*this);
        }

    };
    /* Exception for when an argument is expected to be Optional, but isn't. Not to be confused with the OptionMismatchException. */
    class OptionalMismatchException : public MismatchException {
    private:
        /* The name of the argument that wasn't optional. */
        std::string name;

    public:
        OptionalMismatchException(const std::string& context, const std::string& name) :
            MismatchException(context, generate_message(context, "Argument '" + name + "' is expected to be optional, but is in fact mandatory.")),
            name(name)
        {}

        /* Returns the name of the argument that could be specified multiple times against our expectations. */
        inline std::string get_name() const { return this->name; }

        /* Allows derived classes to copy themselves. */
        virtual OptionalMismatchException* copy() const {
            return new OptionalMismatchException(*this);
        }

    };

    /* Baseclass for all exceptions that have to do with an identifier being unknown. */
    class UnknownException : public ProgrammingException {
    public:
        /* Constructor for the UnknownException which takes a context and optionally a message. */
        UnknownException(const std::string& context, const std::string& message = "") :
            ProgrammingException(context, message)
        {}

        /* Allows derived classes to copy themselves. */
        virtual UnknownException* copy() const {
            return new UnknownException(*this);
        }
    };
    /* Exception for when an argument accessed from the Arguments dict is unknown. */
    class UnknownNameException : public UnknownException {
    private:
        /* The name that did not match with an argument. */
        std::string name;

    public:
        /* Constructor for the UnknownNameException which takes a context and the unknown uid. */
        UnknownNameException(const std::string& context, const std::string& name) :
            UnknownException(context, generate_message(context, "Could not find argument with name '" + name + "'")),
            name(name)
        {}

        /* Returns the name that couldn't be matched to a known argument. */
        inline std::string get_name() const { return this->name; }

        /* Allows derived classes to copy themselves. */
        virtual UnknownNameException* copy() const {
            return new UnknownNameException(*this);
        }
    };

    /* Baseclass for all exceptions that have to do with an identifier being duplicate. */
    class DuplicateException : public ProgrammingException {
    public:
        /* Constructor for the DuplicateException which takes a context and optionally a message. */
        DuplicateException(const std::string& context, const std::string& message = "") :
            ProgrammingException(context, message)
        {}

        /* Allows derived classes to copy themselves. */
        virtual DuplicateException* copy() const {
            return new DuplicateException(*this);
        }

    };
    /* Exception for when a name of an argument is already defined. */
    class DuplicateNameException : public DuplicateException {
    private:
        /* The name that already existed. */
        std::string name;

    public:
        /* Constructor for the DuplicateNameException which takes a context and the name that was a duplicate. */
        DuplicateNameException(const std::string& context, const std::string& name) :
            DuplicateException(context, generate_message(context, "An argument with name '" + name + "' already exists.")),
            name(name)
        {}

        /* Returns the name that already existed. */
        inline std::string get_name() const { return this->name; }

        /* Allows derived classes to copy themselves. */
        virtual DuplicateNameException* copy() const {
            return new DuplicateNameException(*this);
        }

    };
    /* Exception for when the shortlabel of an argument is already defined. */
    class DuplicateShortlabelException : public DuplicateException {
    private:
        /* The shortlabel that already existed. */
        char shortlabel;

    public:
        /* Constructor for the DuplicateShortlabelException which takes a context and the name that was a duplicate. */
        DuplicateShortlabelException(const std::string& context, const char shortlabel) :
            DuplicateException(context, generate_message(context, (std::string("An argument with shortlabel '") += shortlabel) + "' already exists.")),
            shortlabel(shortlabel)
        {}

        /* Returns the shortlabel that already existed. */
        inline char get_shortlabel() const { return this->shortlabel; }

        /* Allows derived classes to copy themselves. */
        virtual DuplicateShortlabelException* copy() const {
            return new DuplicateShortlabelException(*this);
        }

    };

    /* Baseclass exception for all exceptions that point to some illegal value at the programming side. */
    class IllegalException : public ProgrammingException {
    public:
        /* Constructor for the IllegalException which takes a context and optionally a message. */
        IllegalException(const std::string& context, const std::string& message = "") :
            ProgrammingException(context, message)
        {}

        /* Allows derived classes to copy themselves. */
        virtual IllegalException* copy() const {
            return new IllegalException(*this);
        }

    };
    /* Exception for when a name contains an illegal character. */
    class IllegalNameException : public IllegalException {
    private:
        /* The name that contained the illegal character. */
        std::string name;
        /* The character that was illegal. */
        char illegal_char;
    
    public:
        /* Constructor for the IllegalShortlabelException which takes a context and the illegal character. */
        IllegalNameException(const std::string& context, const std::string& illegal_name, const char illegal_char) :
            IllegalException(context, generate_message(context, "A name can only consists of 0-9, a-z and A-Z characters.")),
            name(illegal_name),
            illegal_char(illegal_char)
        {}

        /* Returns the name that contained an illegal character. */
        inline std::string get_illegal_name() const { return this->name; }
        /* Returns the character that was illegal. */
        inline char get_illegal_char() const { return this->illegal_char; }

        /* Allows derived classes to copy themselves. */
        virtual IllegalNameException* copy() const {
            return new IllegalNameException(*this);
        }

    };
    /* Exception for when a shortlabel consists of an illegal character. */
    class IllegalShortlabelException : public IllegalException {
    private:
        /* The character that was illegal. */
        char illegal_char;
    
    public:
        /* Constructor for the IllegalShortlabelException which takes a context and the illegal character. */
        IllegalShortlabelException(const std::string& context, const char illegal_char) :
            IllegalException(context, generate_message(context, "Shortlabel can only consists of 0-9, a-z and A-Z characters.")),
            illegal_char(illegal_char)
        {}

        /* Returns the character that was illegal. */
        inline char get_illegal_char() const { return this->illegal_char; }

        /* Allows derived classes to copy themselves. */
        virtual IllegalShortlabelException* copy() const {
            return new IllegalShortlabelException(*this);
        }

    };
    /* Exception for when an illegal default value was given. */
    class IllegalDefaultValueException : public IllegalException {
    private:
        /* The name of the argument that was given an illegal default value. */
        std::string name;
        /* The value (as any) of the argument that was given an illegal default value. */
        std::any value;
    
    public:
        /* Constructor for the IllegalDefaultValueException that takes a context, the name of the unfortunate argument and the illegal value itself. */
        IllegalDefaultValueException(const std::string& context, const std::string& name, const std::any& value) :
            IllegalException(context, generate_message(context, "Argument '" + name + "' cannot have a default value of type '" + value.type().name() + "'.")),
            name(name),
            value(value)
        {}

        /* Returns the name of the argument that was given an illegal default value. */
        inline std::string get_name() const { return this->name; }
        /* Returns the value that was illegally given to the argument. */
        inline std::any get_value() const { return this->value; }

        /* Allows derived classes to copy themselves. */
        virtual IllegalDefaultValueException* copy() const {
            return new IllegalDefaultValueException(*this);
        }

    };
    /* Exception for when the entered n_values is invalid. */
    class IllegalNValuesException : public IllegalException {
    private:
        /* The value that was illegal. */
        size_t n_values;
    
    public:
        /* Constructor for the IllegalNValuesException that takes a context and the illegal value of DerivedType's n_value field. */
        IllegalNValuesException(const std::string& context, const size_t n_values) :
            IllegalException(context, generate_message(context, "DerivedType::n_values cannot be equal to " + std::to_string(n_values))),
            n_values(n_values)
        {}

        /* Returns the value for DerivedType::n_values that was illegal. */
        inline size_t get_n_values() const { return this->n_values; }

        /* Allows derived classes to copy themselves. */
        virtual IllegalNValuesException* copy() const {
            return new IllegalNValuesException(*this);
        }

    };



/* Exception for when an argument is specified that wasn't allowed to be specified. */
    class ExcludedArgumentException : public ParseException {
    private:
        /* The name of the argument which was duplicate. */
        std::string name;
        /* The shortlabel of the argument which was duplicate. */
        char shortlabel;
    
    public:
        /* Constructor for the ExcludedArgumentException which takes the name of the argument that wasn't allowed to be specified and it's shortlabel. */
        ExcludedArgumentException(const std::string& name, const char shortlabel)
            : ParseException(generate_message("Cannot specify argument --" + name + ", as it is excluded by speficying another argument (see '--help' for details).")),
            name(name),
            shortlabel(shortlabel)
        {};

        /* Returns the name of the duplicate argument. */
        inline std::string get_name() const { return this->name; }
        /* Returns the shortlabel of the duplicate argument. */
        inline char get_shortlabel() const { return this->shortlabel; }

        /* Allows derived classes to copy themselves. */
        virtual ExcludedArgumentException* copy() const {
            return new ExcludedArgumentException(*this);
        }
        
    };
    /* Exception for when an argument is specified twice or more while it could only be specified once. */
    class DuplicateArgumentException : public ParseException {
    private:
        /* The name of the argument which was duplicate. */
        std::string name;
        /* The shortlabel of the argument which was duplicate. */
        char shortlabel;
    
    public:
        /* Constructor for the DuplicateArgumentException which takes the name of the argument and it's shortlabel. */
        DuplicateArgumentException(const std::string& name, const char shortlabel)
            : ParseException(generate_message("Cannot specify argument --" + name + " more than once.")),
            name(name),
            shortlabel(shortlabel)
        {};

        /* Returns the name of the duplicate argument. */
        inline std::string get_name() const { return this->name; }
        /* Returns the shortlabel of the duplicate argument. */
        inline char get_shortlabel() const { return this->shortlabel; }

        /* Allows derived classes to copy themselves. */
        virtual DuplicateArgumentException* copy() const {
            return new DuplicateArgumentException(*this);
        }
        
    };
    /* Exception for when a mandatory argument is missing. */
    class MissingMandatoryException : public ParseException {
    private:
        /* The name of the missing argument. */
        std::string name;

    public:
        /* Constructor for the MissingMandatoryException, which takes the uid of the missing positionals, a placeholder naming it and its index. */
        MissingMandatoryException(const std::string& name)
            : ParseException(generate_message("Missing mandatory Positional '" + name + "'.")),
            name(name)
        {}
        /* Constructor for the MissingMandatoryException, which takes the uid of the missing options and its shortlabel / longlabel. */
        MissingMandatoryException(const std::string& name, const char shortlabel)
            : ParseException(generate_message("Missing mandatory Option '--" + name + (std::string("' (-") += shortlabel) + ").")),
            name(name)
        {}

        /* Returns the name of the Argument that was missing. */
        inline std::string get_name() const { return this->name; }

        /* Allows derived classes to copy themselves. */
        virtual MissingMandatoryException* copy() const {
            return new MissingMandatoryException(*this);
        }

    };

    /* Baseclass exception for when a label is unknown. */
    class UnknownLabelException : public ParseException {
    public:
        /* Constructor for the UnknownLabelException which optionally takes a message. */
        UnknownLabelException(const std::string& message = "") :
            ParseException(message)
        {}

        /* Allows derived classes to copy themselves. */
        virtual UnknownLabelException* copy() const {
            return new UnknownLabelException(*this);
        }

    };
    /* Exception for when given shortlabel is unknown. */
    class UnknownShortlabelException : public UnknownLabelException {
    private:
        /* The shortlabel that was unknown. */
        char shortlabel;

    public:
        /* Constructor for the UnknownLabelException which takes the shortlabel that was unknown. */
        UnknownShortlabelException(const char shortlabel) :
            UnknownLabelException(generate_message((std::string("Unknown argument '-") += shortlabel) + "'.")),
            shortlabel(shortlabel)
        {}

        /* Returns the unknown shortlabel. */
        inline char get_shortlabel() const { return this->shortlabel; }

        /* Allows derived classes to copy themselves. */
        virtual UnknownShortlabelException* copy() const {
            return new UnknownShortlabelException(*this);
        }

    };
    /* Exception for when given longlabel (name) is unknown. */
    class UnknownLonglabelException : public UnknownLabelException {
    private:
        /* The longlabel that was unknown. */
        std::string longlabel;

    public:
        /* Constructor for the UnknownLabelException which takes the longlabel that was unknown. */
        UnknownLonglabelException(const std::string& longlabel) :
            UnknownLabelException(generate_message("Unknown argument '--" + longlabel + "'.")),
            longlabel(longlabel)
        {}

        /* Returns the unknown longlabel. */
        inline std::string get_longlabel() const { return this->longlabel; }

        /* Allows derived classes to copy themselves. */
        virtual UnknownLonglabelException* copy() const {
            return new UnknownLonglabelException(*this);
        }

    };

    /* Baseclass exception for when something is wrong during type parsing. */
    class TypeParseException : public ParseException {
    private:
        /* The message that any derived class gave us. */
        std::string message;

    protected:
        /* The name of the type that errored. */
        std::string type_name;
        /* The name of the argument where something went wrong. */
        std::string name;

        /* Returns the message of a derived class plus whatever the parent classes wants to add. This overload is for Positionals. */
        static inline std::string generate_message(const std::string& type_name, const std::string& name, const std::string& message) {
            return ParseException::generate_message("Could not parse value of the Positional '" + name + "' to " + type_name + ": " + message);
        }
        /* Returns the message of a derived class plus whatever the parent classes wants to add. This overload is for Options. */
        static inline std::string generate_message(const std::string& type_name, const std::string& name, const char shortlabel, const std::string& message) {
            return ParseException::generate_message("Could not parse value of the '--" + name + (std::string("' (-") += shortlabel) + ") to " + type_name + ": " + message);
        }

        /* Declare the parse functions as friend. */
        friend class Positional;
        friend class Option;

    public:
        /* Constructor for the TypeParseException which takes the name of the type that was incorrectly parsed and optionally a message. Use extend() to inject the uid. */
        TypeParseException(const std::string& type_name, const std::string& message = "") :
            ParseException(message),
            message(message),
            type_name(type_name),
            name("")
        {}
        /* Constructor for the TypeParseException which takes the name of the type that was incorrectly parsed, the uid of the Argument and optionally a message. */
        TypeParseException(const std::string& type_name, const std::string& name, const std::string& message) :
            ParseException(message),
            message(message),
            type_name(type_name),
            name(name)
        {}

        /* Returns the name of the type that was wrongly parsed. */
        inline std::string get_type_name() const { return this->type_name; }
        /* Returns the name of the Argument that was wrongly parsed. */
        inline std::string get_name() const { return this->name; }

        /* Allows derived classes to copy themselves. */
        virtual TypeParseException* copy() const {
            return new TypeParseException(*this);
        }

    };
    /* Exception for when an illegal character is encountered while parsing a Positional. */
    class IllegalCharException : public TypeParseException {
    protected:
        /* The character that was illegal. */
        char c;

    public:
        /* Constructor which takes the name of the type and the illegal character. Use extend() to inject the uid. */
        IllegalCharException(const std::string& type_name, const char illegal_char) :
            TypeParseException(type_name, (std::string("Illegal character '") += illegal_char) + "' encountered"),
            c(illegal_char)
        {}
        /* Constructor which takes the name of the type, the illegal character and the index of the Positional. */
        IllegalCharException(const std::string& type_name, const char illegal_char, const std::string& name) :
            TypeParseException(type_name, name, generate_message(type_name, name, (std::string("Illegal character '") += illegal_char) + "' encountered")),
            c(illegal_char)
        {}
        /* Constructor which takes the name of the type, the illegal character and the shortlabel / longlabel of the Option. */
        IllegalCharException(const std::string& type_name, const char illegal_char, const std::string& name, const char shortlabel) :
            TypeParseException(type_name, name, generate_message(type_name, name, shortlabel, (std::string("Illegal character '") += illegal_char) + "' encountered")),
            c(illegal_char)
        {}

        /* Returns the character that was illegal. */
        inline char get_c() const { return this->c; }

        /* Allows derived classes to copy themselves. */
        virtual IllegalCharException* copy() const {
            return new IllegalCharException(*this);
        }

    };
    /* Exception for when an invalid value is given (string rather than char). */
    class IllegalValueException : public TypeParseException {
    protected:
        /* The value that was illegal. */
        std::string value;

    public:
        /* Constructor which takes the name of the type and the illegal value. Use extend() to inject the uid. */
        IllegalValueException(const std::string& type_name, const std::string& illegal_value) :
            TypeParseException(type_name, "Illegal value '" + illegal_value + "' encountered"),
            value(illegal_value)
        {}
        /* Constructor which takes the name of the type, the illegal character and the index of the Positional. */
        IllegalValueException(const std::string& type_name, const std::string& illegal_value, const std::string& name) :
            TypeParseException(type_name, name, generate_message(type_name, name, "Illegal value '" + illegal_value + "' encountered")),
            value(illegal_value)
        {}
        /* Constructor which takes the name of the type, the illegal character and the shortlabel / longlabel of the Option. */
        IllegalValueException(const std::string& type_name, const std::string& illegal_value, const std::string& name, const char shortlabel) :
            TypeParseException(type_name, name, generate_message(type_name, name, shortlabel, "Illegal value '" + illegal_value + "' encountered")),
            value(illegal_value)
        {}

        /* Returns the string that was illegal. */
        inline std::string get_value() const { return this->value; }

        /* Allows derived classes to copy themselves. */
        virtual IllegalValueException* copy() const {
            return new IllegalValueException(*this);
        }

    };
    /* Exception for when a parsed value is out of bounds for some reason. */
    class OutOfBoundsException : public TypeParseException {
    protected:
        /* The boundry that should not be exceeded (in string). */
        std::string boundry;
        /* The value that was attempted to be parsed. */
        std::string value;

    public:
        /* Constructor which takes the type that was attempted to be parsed, the boundry that should not have been exceeded and the value that was given. The uid can be set using extend(). */
        OutOfBoundsException(const std::string& type_name, const std::string& boundry_value, const std::string& given_value)
            : TypeParseException(type_name, "Value '" + given_value + "' is out-of-bounds for bounding value " + boundry_value),
            boundry(boundry_value),
            value(given_value)
        {}
        /* Constructor which takes the type that was attempted to be parsed, the boundry that should not have been exceeded, the value that was given, the uid of the argument and the index of the Positional that is the argument. */
        OutOfBoundsException(const std::string& type_name, const std::string& boundry_value, const std::string& given_value, const std::string& name)
            : TypeParseException(type_name, name, generate_message(type_name, name, "Value '" + given_value + "' is out-of-bounds for bounding value " + boundry_value)),
            boundry(boundry_value),
            value(given_value)
        {}
        /* Constructor which takes the type that was attempted to be parsed, the boundry that should not have been exceeded, the value that was given, the uid of the argument and the shortlabel / longlabel of the Option that is the argument. */
        OutOfBoundsException(const std::string& type_name, const std::string& boundry_value, const std::string& given_value, const std::string& name, const char shortlabel)
            : TypeParseException(type_name, name, generate_message(type_name, name, shortlabel, "Value '" + given_value + "' is out-of-bounds for bounding value " + boundry_value)),
            boundry(boundry_value),
            value(given_value)
        {}

        /* Returns the boundry that should not have been exceeded (as a string). */
        inline std::string get_boundry() const { return this->boundry; }
        /* Returns the value that failed to be parsed. */
        inline std::string get_value() const { return this->value; }

        /* Allows derived classes to copy themselves. */
        virtual OutOfBoundsException* copy() const {
            return new OutOfBoundsException(*this);
        }

    };

