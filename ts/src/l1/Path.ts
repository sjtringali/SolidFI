// Copyright (c) 2026 Scott Tringali. All rights reserved.

import { Converter } from './Converter';
import { Parameters } from './Parameters';
import { Failed } from './Chain';

interface PathNode<In, Out, P extends Parameters> {
    converter: Converter<In, Out, P>;
    next: PathNode<Out, any, P> | null;
}

// Not exported: callers use new Path() for the null default, or Failed(value) for an explicit sentinel.
const NullFailed = Failed<any>(null);

export class Path<T, U, P extends Parameters = Parameters> implements Converter<T, U, P> {
    readonly pathFailed: Failed<U>;
    private head: PathNode<T, any, P> | null = null;
    private tail: PathNode<any, U, P> | null = null;

    constructor(failed: Failed<U> = NullFailed) {
        this.pathFailed = failed;
    }

    static create<T, U, P extends Parameters = Parameters>(...converters: Converter<any, any, any>[]): Path<T, U, P> {
        return converters.reduce(
            (path, c) => path.addNode(c),
            new Path<T, U, P>()
        );
    }

    // TODO: type mismatches between steps are undetected; requires an interior builder to catch at append time.
    append<I>(converter: Converter<U, I, P>): Path<T, I, P> {
        const next = new Path<T, I, P>();
        let node = this.head;
        while (node !== null) {
            next.addNode(node.converter);
            node = node.next;
        }
        next.addNode(converter);
        return next;
    }

    private addNode(converter: Converter<any, any, any>): this {
        const node: PathNode<any, any, P> = { converter, next: null };
        if (this.tail) {
            this.tail.next = node;
        } else {
            this.head = node;
        }
        this.tail = node;
        return this;
    }

    traverse(value: T, params: P): U {
        if (this.head === null) {
            return this.pathFailed.value;
        }
        // Type-safe at the boundary, but the cast here is for simplicity.
        // We could replace this with a hidden builder inside, for completeness.
        let current: any = value;
        let node: PathNode<any, any, P> | null = this.head;
        while (node !== null) {
            current = node.converter.resolve(current, params);
            node = node.next;
        }
        return current as U;
    }

    resolve(value: T, params: P): U {
        return this.traverse(value, params);
    }
}
