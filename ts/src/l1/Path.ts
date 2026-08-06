// Copyright (c) 2026 Scott Tringali. All rights reserved.

import { Converter } from './Converter';
import { Parameters } from './Parameters';
import { Failed } from './Chain';

// Not exported: callers use new Path() for the null default, or Failed(value) for an explicit sentinel.
const NullFailed = Failed<any>(null);

export class Path<T, U, P extends Parameters = Parameters> implements Converter<T, U, P> {
    readonly pathFailed: Failed<U>;
    private head: Path.Step<T, any, P> | null = null;
    private tail: Path.Step<any, U, P> | null = null;

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
    append(converter: Converter<U, any, P>): void {
        this.addNode(converter);
    }

    to<I>(converter: Converter<U, I, P>): Path<T, I, P> {
        const next = new Path<T, I, P>();
        let node = this.head;
        while (node !== null) {
            next.addNode(node.converter);
            node = node.next;
        }
        next.addNode(converter);
        return next;
    }

    protected copyTo<M extends Path<T, any, P>>(target: M): M {
        let node = this.head;
        while (node !== null) {
            target.addNode(node.converter);
            node = node.next;
        }
        return target;
    }

    protected addNode(converter: Converter<any, any, any>): this {
        const node: Path.Step<any, any, P> = { converter, next: null };
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
        let current: any = value;
        let node: Path.Step<any, any, P> | null = this.head;
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

export namespace Path {
    export interface Step<In, Out, P extends Parameters> {
        converter: Converter<In, Out, P>;
        next: Step<Out, any, P> | null;
    }
}
