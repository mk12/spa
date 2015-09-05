# SPA

SPA is a Simple Proof Assistant. It can help you prove basic theorems about sets and numbers.

## Usage

Build the project with `./build.sh` and then run `./dist/spa` to enter the interactive console. If all goes well, it should greet you with a friendly banner:

```
 _____   _____   _____
|  ___| |  _  | |  _  |  |  Simple Proof Assisant
| |___  | |_| | | |_| |  |  (c) 2015 Mitchell Kember
|___  | |  ___| |  _  |  |  Version 0.1
 ___| | | |     | | | |  |
|_____| |_|     |_| |_|  |  Type "help" to get started
```

Enter `help` to see the commands that are available.

## Proving a theorem

You start by entering your theorem. Here's a convoluted example:

```
prove (forall x in ZZ (iff (and (= x (+ 1 1)) (sub {x} ZZ)) (exists a (= a a))))
```

Theorems are written in prefix notation, similar to Lisp. At any time, you can enter `stat` to see the current status of the proof. In this case, we want to decompose the theorem using `dec`.

After decomposing a few times and then entering `tree`, you'll see the power of SPA: it breaks down the theorem into managable pieces (goals), keeps track of your givens, and lets you navigate the tree until you've proved the original theorem. If you enter `ded`, SPA will automatically derive givens from your current knowledge. When you get to a simple goal such as `(= 1 1)`, you can enter `triv` to let SPA know that it's trivial to prove. You can also enter `just` and provide a sentence of justification.

After proving that theorem, the tree looks like this:

```
                _______________A                               
               /                                               
        _______B                                               
       /                                                       
    ___C___                                                    
   /       \                                                   
  _D      _E                                                   
 /       /                                                     
 F       G                                                     
        / \                                                    
        H I                                                    

[A] (forall x (=> (in x ZZ) (iff (and (= x (+ 1 1)) (sub {x} ZZ)) (exists a (= a a)))))
[B] (=> (in x ZZ) (iff (and (= x (+ 1 1)) (sub {x} ZZ)) (exists a (= a a))))
[C] (iff (and (= x (+ 1 1)) (sub {x} ZZ)) (exists a (= a a)))
[D] (=> (and (= x (+ 1 1)) (sub {x} ZZ)) (exists a (= a a)))
[E] (=> (exists a (= a a)) (and (= x (+ 1 1)) (sub {x} ZZ)))
[F] (exists a (= a a))
[G] (and (= x (+ 1 1)) (sub {x} ZZ))
[H] (= x (+ 1 1))
[I] (sub {x} ZZ)
```

## Objects

There are three types of mathematical objects in SPA:

- Numbers
    - Concrete numbers: 1, -1, 99, etc.
    - Compound numbers: sum, difference, or product.
- Sets
    - Concrete sets: {0}, {0, 42}, etc.
    - Compound sets: union, intersection, or difference.
    - Special sets: empty set, naturals, integers, set of sets.
- Symbols
    - A single character that refers to another object: _x_, _y_, etc.

## Sentences

There are three types of mathematical sentences in SPA:

- Logical (all binary except `not`)
    - Boolean algebra: `and`, `or`, `not`, `=>` (implies), or `iff` (if and only if).
- Relation (all binary)
    - `=`, `!=`, `<`, `>`, `<=`, `>=`, `s=` (set equality), `sub` (proper subset), `sube` (subset), `in` (belongs to), `div` (divides).
- Quantified
    - Existential: `(exists a (> a 5))`.
    - Universal: `(forall a (= a a))`.

There is a short form for quantified sentences:

- `(forall x in NN (!= x -1))` is equivalent to `(forall x (=> (in x NN) (!= x -1)))`.
- `(exists x in ZZ (= x -1))` is equivalent to `(exists x (and (in x ZZ) (= x -1)))`.

## Usefulness

In the future SPA may become intelligent enought to prove basic theorems on its own. For now, it just manages mechanical steps of theorem-proving: the current goal, the tree of goals it fits into, the list of givens, the rules of decomposition, and the rules of deduction.

This is useful for some theorems you might see in Algebra class. In the real world, though, theorems are hard to prove not because they are long or have deeply nested trees — in fact, many difficult theorems consist of a single quantified implication — but because they involve a very difficult step. For example, SPA will never be able to help you prove the existence of an object.

All that being said, I enjoyed writing SPA and I hope others have fun playing around with it.

## Contributions

Contributions are welcome! Take a look at the current issues to see what needs to be worked on. Please create a pull request if you have anything to contribute.

## License

© 2015 Mitchell Kember

SPA is available under the MIT License; see [LICENSE](LICENSE.md) for details.
