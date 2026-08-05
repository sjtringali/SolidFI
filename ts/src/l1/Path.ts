// Copyright (c) 2026 Scott Tringali. All rights reserved.

import { Converter } from './Converter';
import { Parameters } from './Parameters';
import { Failed } from './Chain';

interface PathNode {
    converter: Converter<any, any, any>;
    next: PathNode | null;
}

// Not exported: callers use new Path() for the null default, or Failed(value) for an explicit sentinel.
const NullFailed = Failed<any>(null);

export class Path<T, U, P extends Parameters = Parameters> implements Converter<T, U, P> {
    readonly pathFailed: Failed<U>;
    private head: PathNode | null = null;
    private tail: PathNode | null = null;

    constructor(failed: Failed<U> = NullFailed) {
        this.pathFailed = failed;
    }

    static create<T, U, P extends Parameters = Parameters>(...converters: Converter<any, any, any>[]): Path<T, U, P> {
        const path = new Path<T, U, P>();
        for (const c of converters) {
            path.append(c);
        }
        return path;
    }

    // TODO: type mismatches between steps are undetected; requires an interior builder to catch at append time.
    append(converter: Converter<any, any, P>): void {
        const node: PathNode = { converter, next: null };
        if (this.tail) {
            this.tail.next = node;
        } else {
            this.head = node;
        }
        this.tail = node;
    }

    traverse(value: T, params: P): U {
        if (this.head === null) {
            return this.pathFailed.value;
        }
        // Type-safe at the boundary, but the cast here is for simplicity.
        // We could replace this with a hidden builder inside, for completeness.
        let current: any = value;
        let node: PathNode | null = this.head;
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
