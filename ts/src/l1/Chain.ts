// Copyright (c) 2026 Scott Tringali. All rights reserved.

import { Converter } from './Converter';
import { Parameters } from './Parameters';

export interface Failed<U> {
    readonly value: U;
}

export function Failed<U>(value: U): Failed<U> {
    return { value };
}

export namespace Failed {
    export const Null: Failed<any> = { value: null };
}

export class Chain<T, U, P extends Parameters = Parameters> implements Converter<T, U, P> {
    readonly chainFailed: Failed<U>;
    private entries: Chain.Strategy<T, U, P>[] = [];

    constructor(failed: Failed<U> = Failed.Null) {
        this.chainFailed = failed;
    }

    install(priority: number, name: string, converter: Converter<T, U, P>, failed?: U): void {
        if (this.entries.some(e => e.priority === priority)) {
            throw new Error(`Duplicate priority ${priority}`);
        }
        this.entries.push({ priority, name, converter, failed });
        this.entries.sort((a, b) => a.priority - b.priority);
    }

    installNew<Args extends unknown[]>(priority: number, name: string, ctor: new (...args: Args) => Converter<T, U, P>, ...args: Args): void {
        this.install(priority, name, new ctor(...args));
    }

    resolve(value: T, params: P): U {
        for (const { converter, failed: linkFailed } of this.entries) {
            const { accepts, rejects, handles } = converter;
            if (handles && !handles(params)) {
                continue;
            }
            if (rejects && rejects(value)) {
                continue;
            }
            if (accepts && !accepts(value)) {
                continue;
            }
            const result = converter.resolve(value, params);
            if (result !== (linkFailed ?? this.chainFailed.value)) {
                return result;
            }
        }
        return this.chainFailed.value;
    }
}

namespace Chain {
    export interface Strategy<T, U, P extends Parameters> {
        priority: number;
        name: string;
        converter: Converter<T, U, P>;
        failed?: U;
    }
}
